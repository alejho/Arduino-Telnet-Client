# Arduino-Telnet-Client
a minimal (but real) telnet client for Arduino, actually able to login and send commands to a generic telnet server.

<h1>Getting Started:</h1>
It's a normal Arduino library...so just install it and take a look at the example provided to understand ho to use it!<br>
<a href="https://www.arduino.cc/en/Guide/Libraries">how to install an Arduino library</a>

<h1>Useful tips:</h1>

<b>By default</b> the connection is made on <b>port 23</b>, but you can specify a different port passing it as optional argument to the login method.

<b>By default</b> the answer received from the server is <b>printed on serial port</b>, but you can <b>edit just the print method in TelnetClient.cpp to redirect the chars received</b> from the server as you prefer.

