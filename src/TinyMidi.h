

class TinyMidi {

    void (*noteOnCallback)(byte channel, byte note, byte velocity);
    void (*noteOffCallback)(byte channel, byte note);
    void (*controlChangeCallback) (byte channel, byte controller, byte value);

  private:
    byte  midiType;
    byte midiChannel;

    uint8_t midiMessageLength = 0;
    byte midiMessage[3];
    Stream * stream;

  public:
    const static byte NOTE_OFF = 0x08;
    const static byte NOTE_ON = 0x09;
    const static byte CTL = 0x0B;
    const static int BAUD = 31250;
    /*
      #define MM_NOTE_OFF 0x08
      #define MM_NOTE_ON 0x09
      #define MM_CTL 0x0B
      #define MM_BAUD 31250
    */
    TinyMidi( Stream * stream) {
      this->stream = stream;
    }

    void setControlChangeCallback(void (*fptr)(byte channel, byte controller, byte value)) {
      controlChangeCallback = fptr;
    }

    void setMidiNoteOnCallback(void (*fptr)(byte channel, byte note, byte velocity)) {
      noteOnCallback = fptr;
    }

    void setMidiNoteOffCallback(void (*fptr)(byte channel, byte note)) {
      noteOffCallback = fptr;
    }

    void receiveMessages() {
      while (stream->available() ) {
        uint8_t extracted = stream->read();

        if (extracted < 0x80) {
          if ( midiType ) {
            midiMessage[midiMessageLength] = extracted;
            midiMessageLength++;
            if ( midiMessageLength == 2 ) {
              if ( midiType == NOTE_ON  && noteOnCallback  ) {

                noteOnCallback(midiChannel, midiMessage[0], midiMessage[1]);

              } else if ( midiType == NOTE_OFF && noteOffCallback   ) {

                noteOffCallback(midiChannel, midiMessage[0] );

              } else if ( midiType == CTL && controlChangeCallback  ) {

                controlChangeCallback(midiChannel, midiMessage[0] , midiMessage[1]);

              }
              midiType = 0;
            }
          }
        } else {
          midiChannel = extracted & B00001111;
          // GET RID OF CHANNEL DATA
          extracted = extracted >> 4;
          // ONLY HANDLE KNOWN MESSAGES
          if ( extracted == NOTE_OFF || extracted == NOTE_ON || extracted == CTL ) {
            midiType = extracted;
          } else {
            midiType = 0;
          }
          midiMessageLength = 0;
        }
      }
    }
};
