const express = require('express');
const http = require('http');
const WebSocket = require('ws');
const mqtt = require('mqtt');
const path = require('path');

const app = express();
const server = http.createServer(app);
const wss = new WebSocket.Server({ server });

app.use(express.static(path.join(__dirname, 'public')));

let receivedData = [];

const mqttClient = mqtt.connect('mqtt://test.mosquitto.org');

mqttClient.on('connect', () => {
  console.log('Conectado al broker MQTT');
  mqttClient.subscribe('LMRMP');
});

mqttClient.on('message', (topic, message) => {
  try {
    const data = JSON.parse(message.toString());
    const formattedData = {
      timestamp: new Date().toLocaleString(),
      equipo: data.Equipo || '4LR',
      contador: data.contador || 0,
      rssi: data.rssi || -999
    };
    
    console.log('Datos recibidos:', formattedData);
    
    receivedData.push(formattedData);
    
    wss.clients.forEach(client => {
      if (client.readyState === WebSocket.OPEN) {
        client.send(JSON.stringify(formattedData));
      }
    });
  } catch (error) {
    console.error('Error procesando mensaje MQTT:', error);
  }
});

app.get('/', (req, res) => {
  res.sendFile(path.join(__dirname, 'public', 'index.html'));
});

app.get('/export', (req, res) => {
  if (receivedData.length === 0) {
    return res.status(404).send('No hay datos para exportar');
  }
  
  let csv = 'Timestamp,Equipo,Contador,RSSI\n';
  
  receivedData.forEach(item => {
    csv += `"${item.timestamp}","${item.equipo}",${item.contador},${item.rssi}\n`;
  });
  
  res.setHeader('Content-Type', 'text/csv');
  res.setHeader('Content-Disposition', 'attachment; filename=lora-datos.csv');
  res.send(csv);
});

const PORT = process.env.PORT || 3000;
server.listen(PORT, () => {
  console.log(`Servidor ejecutándose en http://localhost:${PORT}`);

});
