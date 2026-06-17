import threading, time, customtkinter as ctk, serial, serial.tools.list_ports, pystray, sys, psutil, pynvml, json
from PIL import Image, ImageDraw

class HardwareMonitorApp:
    def __init__(self):
        pynvml.nvmlInit()
        self.selected_port = None
        self.is_running = False
        self.root = ctk.CTk()
        self.root.title("GRM")
        self.root.protocol('WM_DELETE_WINDOW', self.minimize_to_tray)
        self.root.geometry("250x175")
        self.create_widgets()
        self.tray_icon = None
        self.baud_rate = 115200;
        self.init_system_tray()

    def create_widgets(self):
        ports = [p.device for p in serial.tools.list_ports.comports()]
        self.choose_lbl = ctk.CTkLabel(self.root, text="Select COM Port For ESP32")
        self.choose_lbl.pack(pady=5)
        self.port_dropdown = ctk.CTkOptionMenu(self.root, values=ports or ["No Ports"], command=lambda c: setattr(self, 'selected_port', c))
        self.port_dropdown.pack(pady=10)
        self.btn_toggle = ctk.CTkButton(self.root, text="Start", command=self.toggle_broadcast)
        self.btn_toggle.pack(pady=10)
        self.status_lbl = ctk.CTkLabel(self.root, text="Stopped")
        self.status_lbl.pack(pady=5)

    def toggle_broadcast(self):
        if not self.is_running:
            self.is_running = True
            self.btn_toggle.configure(text="Stop", fg_color="red")
            self.status_lbl.configure(text="Broadcasting")
            self.worker_thread = threading.Thread(target=self.serial_worker, daemon=True).start()
        else:
            self.is_running = False
            self.btn_toggle.configure(text="Start", fg_color=["#3a7ebf", "#1f538d"])
            self.status_lbl.configure(text="Stopped")


    def serial_worker(self):
        try:
            with serial.Serial(self.selected_port, 115200, timeout=1) as ser:
                time.sleep(1)
                while self.is_running:
                    cpu_percent = psutil.cpu_percent(interval=0.5)
                    gpu_handle = pynvml.nvmlDeviceGetHandleByIndex(0)
                    gpu_name = pynvml.nvmlDeviceGetName(gpu_handle)
                    gpu_temp = pynvml.nvmlDeviceGetTemperature(gpu_handle, pynvml.NVML_TEMPERATURE_GPU)
                    mem_info = pynvml.nvmlDeviceGetMemoryInfo(gpu_handle)
                    ram_percentage = psutil.virtual_memory().percent;
                    vram_percentage = ((mem_info.used / mem_info.total) * 100)
                    packet_data = {"success": True, "CpuPercentage": cpu_percent, "RamPercentage": ram_percentage, "GpuName": gpu_name, "GpuTemp": gpu_temp, "VramPercentage": vram_percentage}
                    json_message = json.dumps(packet_data) + "\n"; #\n is necessary to indicate on the client end when we're done sending a message 
                    ser.write(json_message.encode('utf-8'))
                    print(json_message);
                    time.sleep(0.5)
        except Exception as e:
            self.root.after(0, lambda: self.status_lbl.configure(text=str(e)))

    def init_system_tray(self):
        image = Image.new('RGB', (64, 64), color='blue')
        menu = pystray.Menu(pystray.MenuItem("Show", self.restore), pystray.MenuItem("Exit", self.exit_app))
        self.tray_icon = pystray.Icon("HM", image, "HW Monitor", menu)

    def minimize_to_tray(self):
        self.root.withdraw()
        threading.Thread(target=self.tray_icon.run, daemon=True).start()

    def restore(self, icon, item):
        self.tray_icon.stop()
        self.root.after(0, self.root.deiconify)

    def exit_app(self, icon, item):
        self.is_running = False
        self.tray_icon.stop()
        self.root.after(0, self.root.destroy)

if __name__ == "__main__":
    app = HardwareMonitorApp()
    app.root.mainloop()