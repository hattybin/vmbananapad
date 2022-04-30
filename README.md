# vmbananapad

arduino code for a teensy 4.1 midi controller specifically designed for use with voicemeeter bannana and simple broadcasting for streaming or video conferencing

voice meeter banana has 10 channels. I designed this console for my use and it may not apply for everyone, but I think this is a common application:

- 3 "physical" inputs
    --- mic input from user
    --- input for desktop audio (browser, specific applications, etc)
    --- input for game audio
  
  - 2 virtual inputs
    --- VoiceMeeter Input - input for "catchall" audio from system (system notifications, etc) 
    --- VoiceMeeter Aux Input - input for voice input (discord, video conferencing, etc)
  
  - 3 "physical" outputs
    --- A1 - physical sound card using stable driver (requirement of voicemeeter to run well)
    --- A2 - output to wired headset (backup for me when wireless headset is not charged, etc)
    --- A3 - output to wireless headset (2.4ghz, bluetooth)
  
  - 2 virtual outputs
    --- B1 - VoiceMeeter Output
    --- B2 - VoiceMeeter Aux Output
    
    The physical console has:
      - 8 dedicated sliders
        --- mic in volume (slightly offset from the other 4 input channels)
        --- virtual input 2
        --- virtual input 3
        --- physical input 1
        --- physical input 2
        --- physical output 1 (A1)
        --- physical output 2 (A2)
        --- physical output 3 (A3)
        
      - 2 rotary knobs on channel 1 (my mic in)
        --- Compression and Noise Gate adjustments but can be assigned to anything
        
      - 24 dedicated switches
        --- mic mute for mic in channel
        --- mute, A1, A2, A3 buttons for each virtual and physical input
        --- mute buttons for each phsyical output (A1, A2, A3)
        --- 4 misc assignable switches
            ---- I use 1 for a Voicemeeter macro to 'Reset Audio Engine' when I connect a bluetooth headset.
        
