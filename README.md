ledmaster
=========

This is my LED control system prototype. It uses client-server model.
I was asked to do it using FIFOs, so I have to use two of them: one for
commands going to the server and another one for server replies. Clients
running in parallel will conquer for these two so each client has to obtain
a lock before it can do anything.

Start server by plain: ./server

It should show a single "LED" on the console.

```
Client's usage: ./client [OPTIONS]
Options:
  -s|--state on|off            Turn led on or off
  -c|--color red|green|blue    Set led color
  -r|--rate 0..5               Set led blinking rate
  -h|--help                    Show this help text
```

If started without options client will just query the server's state.
