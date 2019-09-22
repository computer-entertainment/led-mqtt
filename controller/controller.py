#!/usr/bin/env python3

import paho.mqtt.client as mqtt
import struct


class LedState:
    def __init__(self):
        super().__init__()

        self.animationId = 1
        self.brightness = 60
        self.animationSize = 10
        self.speed = 3000
        self.color = [0, 255, 0]
        self.colorRotation = 0.0
        self.decay = 255
        self.fadeId = 0
        self.fadeSpeed = 0

    def toBytes(self):
        self.brightness = min(self.brightness, 80)
        ba = struct.pack("<BBHHBBBxfBBH", self.animationId,
                         self.brightness, self.animationSize,
                         self.speed, self.color[2], self.color[1],
                         self.color[0], self.colorRotation,
                         self.decay, self.fadeId, self.fadeSpeed)
        return ba

    def merge_remote(self, bytes):
        self.color[0], self.color[1], self.color[2], self.brightness, self.speed, self.decay = struct.unpack("<iiiiii", bytes)
        self.speed = self.speed * 2


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Alle subscriptions direkt nach dem Verbinden erneuern
    client.subscribe("input/#")
    client.subscribe("remote/options")
    client.subscribe("remote/animationmodus")


def on_message(client, userdata, msg):
    print("on_message")
    try:
        print(msg.topic, str(msg.payload))

        if msg.topic.startswith("input/"):
            payload = msg.payload.decode()
            if msg.topic == "input/animationId":
                webState.animationId = int(payload)
            if msg.topic == "input/brightness":
                webState.brightness = int(payload)
                print("foobar")
            if msg.topic == "input/animationSize":
                webState.animationSize = int(payload)
            if msg.topic == "input/speed":
                webState.speed = int(payload)
            if msg.topic == "input/color":
                webState.color = list(map(int, payload.split(",")))
            if msg.topic == "input/colorRotation":
                webState.colorRotation = float(payload)
            if msg.topic == "input/decay":
                webState.decay = int(payload)
            if msg.topic == "input/fadeId":
                webState.fadeId = int(payload)
            if msg.topic == "input/fadeSpeed":
                webState.fadeSpeed = int(payload)

            client.publish("display/ledState", webState.toBytes())
        elif msg.topic == "remote/options":
            webState.merge_remote(msg.payload)
            client.publish("display/ledState", webState.toBytes())
        elif msg.topic == "remote/animationmodus":
            payload = msg.payload.decode()
            modes = {"aus": 0, "Stroboskob": 4, "Knight-Rider": 3, "Rainbow": 2, "Statisch": 1}
            webState.animationId = modes[payload]

            client.publish("display/ledState", webState.toBytes())

    except Exception as e:
        print("Exception")
        print(e)


webState = LedState()

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.connect("172.16.3.193", 1883, 60)
client.loop_forever()
