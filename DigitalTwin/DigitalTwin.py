import paho.mqtt.client as mqtt
import json
import time
import threading
import schedule

class PillBoxDigitalTwin:
    def __init__(self, username, key, broker="io.adafruit.com", port=1883):
        self.username = username
        self.key = key
        self.broker = broker
        self.port = port
        
        # Stato interno del Twin (Modulare e serializzabile)
        self.state = {
            "pill_taken": False,
            "last_update": None,
            "device_source": None
        }

        # Configurazione Client MQTT
        self.client = mqtt.Client()
        self.client.username_pw_set(self.username, self.key)
        
        # Assegnazione dei metodi di callback
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message

    def on_connect(self, client, userdata, flags, rc):
        if rc == 0:
            print(f"[*] Connesso al Broker Adafruit come: {self.username}")
            # Iscrizione ai feed (Topic)
            # Usiamo un formato modulare per i nomi dei feed
            client.subscribe(f"{self.username}/feeds/digital-twin-core") 
            # client.subscribe(f"{self.username}/feeds/pill-status")
        else:
            print(f"[!] Errore di connessione, codice: {rc}")

    def on_message(self, client, userdata, msg):
        """Gestisce i messaggi in entrata e aggiorna lo stato del Twin"""
        try:
            payload = json.loads(msg.payload.decode())
            topic = msg.topic
            print(f"[<-] Ricevuto su {topic}: {payload}")

            # Logica di aggiornamento dello stato
            if "action" in payload and payload["action"] == "confirm_intake":
                self.update_twin_state(True, payload.get("device"))

            if "action" in payload and payload["action"] == "connection":
                print(f'dispositivo {msg.device} connesso')#da implementare sulla scheda
                
        except Exception as e:
            print(f"[!] Errore nel parsing del messaggio: {e}")

    def update_twin_state(self, status, device):
        """Metodo modulare per cambiare lo stato e notificare il sistema"""
        self.state["pill_taken"] = status
        self.state["last_update"] = time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
        self.state["device_source"] = device
        
        print(f"[#] Stato Twin aggiornato: {self.state}")
        
        # Notifica il nuovo stato a tutti i dispositivi (Broadcast)
        # self.publish_state()

    def publish_state(self):
        """Invia lo stato attuale sul feed del Digital Twin"""
        topic = f"{self.username}/feeds/digital-twin-core"
        self.client.publish(topic, json.dumps(self.state), retain=True)
        print(f"[->] Stato pubblicato su {topic}") 

    def send_command(self, target_device, command_type, value):
        """
        Invia un comando specifico a una scheda.
        :param target_device: ID della scheda (es. 'casa' o 'ufficio' o 'all')
        :param command_type: Il tipo di comando (es. 'set_led', 'buzzer')
        :param value: Il valore del comando (es. 'GREEN', 'OFF', 'ON')
        """    
        payload = {
            "target": target_device,
            "command": command_type,
            "val": value,
            "sender": "digital_twin_engine",
            "timestamp": time.strftime("%Y-%m-%dT%H:%M:%SZ", time.gmtime())
        }

        # Topic dove le schede sono in ascolto
        topic = f"{self.username}/feeds/pill-commands"

        self.client.publish(topic, json.dumps(payload))
        print(f"[->] Comando inviato a {target_device}: {command_type} -> {value}")

    def run_scheduler_loop(self): # Rinominata per chiarezza
        """Il loop dello scheduler eseguito nel thread"""
        # Cambia l'orario qui per i tuoi test
        schedule.every().day.at("11:41").do(self.trigger_global_check)
        
        # Consiglio: aggiungi un controllo ogni minuto per vedere se il thread è vivo
        # schedule.every(1).minutes.do(lambda: print("[Twin] Scheduler in esecuzione..."))

        while True:
            schedule.run_pending()
            time.sleep(1)

    def trigger_global_check(self):
        """Il cervello interroga attivamente le schede"""
        print("[!] Ore 10:00 - Avvio controllo globale assunzione...")

        # Invia il comando di controllo a tutte le schede tramite Node-RED
        self.send_command(target_device="all", command_type="request_check", value="NOW")

        # Avvia un timer di cortesia: se tra 10 min è ancora False, invia allarme
        threading.Timer(600, self.check_if_missed).start()

    def check_if_missed(self):
        if not self.state["pill_taken"]:
            print("[!!!] ATTENZIONE: Pillola non rilevata. Invio allarmi.")
            self.send_command(target_device="all", command_type="alarm", value="ON")

    def start(self):
        # Avvia lo scheduler in un thread dedicato
        daemon_sched = threading.Thread(target=self.run_scheduler_loop, daemon=True)
        daemon_sched.start()
        print(f"[*] Thread Scheduler avviato. Ora sistema: {time.strftime('%H:%M:%S')}")

        """Avvia il loop di comunicazione"""
        self.client.connect(self.broker, self.port)
        self.client.loop_forever()

# --- ESEMPIO DI UTILIZZO ---
if __name__ == "__main__":
    # Inserisci qui le tue credenziali Adafruit
    ADAFRUIT_USER = "321758"
    ADAFRUIT_KEY = "aio_Hagi76RS2bGzVtYvHgLOoAGFK5Ez"

    twin = PillBoxDigitalTwin(ADAFRUIT_USER, ADAFRUIT_KEY)
    twin.start()