class LoRaDataVisualizer {
    constructor() {
        this.ws = null;
        this.isConnected = false;
        this.statusElement = document.getElementById('status');
        this.tableBody = document.getElementById('tableBody');
        this.exportBtn = document.getElementById('exportBtn');
        
        this.init();
    }
    
    init() {
        this.setupWebSocket();
        this.setupEventListeners();
    }
    
    setupWebSocket() {
        // Determinar la URL del WebSocket basado en la ubicación actual
        const protocol = window.location.protocol === 'https:' ? 'wss:' : 'ws:';
        const wsUrl = `${protocol}//${window.location.host}`;
        
        this.ws = new WebSocket(wsUrl);
        
        this.ws.onopen = () => {
            this.updateConnectionStatus(true, 'Conectado');
            console.log('Conexión WebSocket establecida');
        };
        
        this.ws.onmessage = (event) => {
            this.handleNewData(JSON.parse(event.data));
        };
        
        this.ws.onclose = () => {
            this.updateConnectionStatus(false, 'Desconectado');
            console.log('Conexión WebSocket cerrada');
            
            // Intentar reconectar después de 5 segundos
            setTimeout(() => {
                this.setupWebSocket();
            }, 5000);
        };
        
        this.ws.onerror = (error) => {
            console.error('Error en WebSocket:', error);
            this.updateConnectionStatus(false, 'Error de conexión');
        };
    }
    
    setupEventListeners() {
        this.exportBtn.addEventListener('click', () => {
            window.open('/export', '_blank');
        });
    }
    
    updateConnectionStatus(connected, message) {
        this.isConnected = connected;
        this.statusElement.textContent = message;
        
        // Limpiar clases existentes
        this.statusElement.classList.remove('status-connecting', 'status-connected', 'status-disconnected');
        
        // Añadir clase apropiada
        if (connected) {
            this.statusElement.classList.add('status-connected');
        } else if (message === 'Conectando...') {
            this.statusElement.classList.add('status-connecting');
        } else {
            this.statusElement.classList.add('status-disconnected');
        }
    }
    
    handleNewData(data) {
        // Insertar nueva fila en la tabla
        const newRow = document.createElement('tr');
        
        newRow.innerHTML = `
            <td>${data.timestamp}</td>
            <td>${data.equipo}</td>
            <td>${data.contador}</td>
            <td>${data.rssi} dBm</td>
        `;
        
        // Añadir la nueva fila al principio de la tabla
        this.tableBody.insertBefore(newRow, this.tableBody.firstChild);
        
        // Limitar la tabla a 100 filas para evitar sobrecarga
        if (this.tableBody.children.length > 100) {
            this.tableBody.removeChild(this.tableBody.lastChild);
        }
        
        // Efecto visual de highlight en la nueva fila
        newRow.classList.add('new-row');
        setTimeout(() => {
            newRow.classList.remove('new-row');
        }, 1000);
    }
}

// Inicializar la aplicación cuando el DOM esté listo
document.addEventListener('DOMContentLoaded', () => {
    new LoRaDataVisualizer();
});