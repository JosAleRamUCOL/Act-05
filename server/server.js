const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const mqtt = require('mqtt');
const path = require('path');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

// Middleware para servir archivos estáticos
app.use(express.static(path.join(__dirname, 'public')));

// Almacenamiento de datos en memoria
let receivedData = [];

// Configuración MQTT - conectar a broker público por defecto
const mqttClient = mqtt.connect('mqtt://test.mosquitto.org');

mqttClient.on('connect', () => {
  console.log('Conectado al broker MQTT');
  mqttClient.subscribe('LMRMP'); // Tópico donde ESP32 publica los datos
});

// Procesar mensajes MQTT
mqttClient.on('message', (topic, message) => {
  try {
    const data = JSON.parse(message.toString());
    // Añadir timestamp y asegurar formato correcto
    const formattedData = {
      timestamp: new Date().toLocaleString(),
      equipo: data.Equipo || '4LR',
      contador: data.contador || 0,
      rssi: data.rssi || -999
    };
    
    console.log('Datos recibidos:', formattedData);
    
    // Guardar datos
    receivedData.push(formattedData);
    
    // Enviar a todos los clientes WebSocket conectados
    wss.clients.forEach(client => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(JSON.stringify(formattedData));
      }
    });
  } catch (error) {
    console.error('Error procesando mensaje MQTT:', error);
  }
});

// Ruta principal - servir la página web
app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

// Ruta para exportar datos como CSV
app.get('/export', (req, res) => {
  if (receivedData.length === 0) {
    return res.status(404).send('No hay datos para exportar');
  }
  
  // Encabezados CSV
  let csv = 'Timestamp,Equipo,Contador,RSSI\n';
  
  // Agregar datos
  receivedData.forEach(item => {
    csv += `"${item.timestamp}","${item.equipo}",${item.contador},${item.rssi}\n`;
  });
  
  // Configurar headers para descarga
  res.setHeader('Content-Type', 'text/csv');
  res.setHeader('Content-Disposition', 'attachment; filename=lora-datos.csv');
  res.send(csv);
});

// Iniciar servidor
const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`Servidor ejecutándose en http://localhost:${PORT}`);
});