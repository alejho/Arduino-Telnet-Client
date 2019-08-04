# Arduino-Telnet-Client
a minimal (but real) telnet client for Arduino, <b>actually able to login and send commands to a generic telnet server.</b>

<h2>Getting Started:</h2>
It's a normal Arduino library...so just install it and take a look at the example provided to understand how to use it!<br>
<a href="https://www.arduino.cc/en/Guide/Libraries">how to install an Arduino library</a>

<h2>Useful tips:</h2>

<b>By default</b> the connection is made on <b>port 23</b>, but you can specify a different port passing it as optional argument to the login method.

<b>By default</b> the answer received from the server is <b>printed on serial port</b>, but you can <b>edit just the "print()" method in TelnetClient.cpp to redirect the chars received</b> from the server as you prefer.

<a href="https://imgbb.com/"><img src="https://i.ibb.co/6ykMpDV/print.png" alt="print" border="0"></a>

<b>The only thing you have to configure in your sketch is the char that represent the "prompt"</b> for the server you're connecting to:

<a href="https://imgbb.com/"><img src="https://i.ibb.co/jMJ7LtX/prompt-char.png" alt="prompt-char" border="0"></a>

for this use this function in your sketch:

<a href="https://ibb.co/1n6vZBg"><img src="https://i.ibb.co/VNmjVb1/prompt-char2.png" alt="prompt-char2" border="0"></a>

There are some other <b>configuration parameters</b> you can modify in the header file (Telnet.h).
They are quite self explanatory:

<a href="https://ibb.co/dM5Ljz1"><img src="https://i.ibb.co/V9qQJPr/param.png" alt="param" border="0"></a>

<h2>Messing with a Mikrotik router?</h2>
For some reason I don't understand you're not the first here for this!<br>
I didn't find an explanation for this, but the telnet server implemented on Mikrotik routers asnwers twice...not a big deal, everything works, but your output can be a little messy:

<a href="https://ibb.co/jVCVxdk"><img src="https://i.ibb.co/CJjJXqK/doubleprompt.png" alt="doubleprompt" border="0"></a>

<h2>Known issues:</h2>
It seems that login continuously resetting arduino many times in a short time cause problems at the Ethernet shields...it simply begin to work not properly or to not connect at all.<br>
In this case the only solution seems to disconnect/reconnect the power supply...hope to find a solution for this.

<h2>Troubleshooting:</h2>

For any issue you want to report about this library please uncomment <b>this macro (TNDBG)</b> in the header file

<a href="https://ibb.co/tp4b3Zx"><img src="https://i.ibb.co/XxbzVy3/tndbg.png" alt="tndbg" border="0"></a>

then <b>post the output or your sketch in the <b>Issues section</b> being as specific as possible</b> (share your code, and explain what you're trying to do!).<br>
In general I'm very happy to receive feedback and to help people with their projects...but remember that is just an hobby for me, I'm doing it for free...so words as <b>"please" and "thank you" are really appreciated</b>.
