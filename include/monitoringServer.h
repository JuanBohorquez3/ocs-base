#ifndef MONSERVER
#define MONSERVER

#include <stdint.h>

#define CHANNEL_NAME_SIZE 3 // len('cxx') = 3
#define INT_TYPE_SIZE 3     // len('int') = 3
#define FLOAT_TYPE_SIZE 5     // len('int') = 3
#define TIMESTAMP_SIZE 10   // 2^32 = 4E10

class DataPacket{
  public:
    // copy stream name into class
    DataPacket( uint8_t _channels
        , char* _streamName
        , uint8_t _nameLength
        , uint8_t _entrySize
        , char* _buffer
        , uint8_t _fracSecTS
        )
      : channels(_channels)
      , streamName(_streamName)
      , streamNameSize(_nameLength)
      , dataEntrySize(_entrySize)
      , buffer(_buffer)
      {
        // TODO: find better way
        // calculate regstration packet size
        registerSize = 6 + streamNameSize + channels*( 6 + CHANNEL_NAME_SIZE + INT_TYPE_SIZE );
        //registerSize = 6 + streamNameSize + channels*( 6 + CHANNEL_NAME_SIZE + FLOAT_TYPE_SIZE );
        // calculate data packet size
        packetSize = 7 + streamNameSize + TIMESTAMP_SIZE + channels*( 4 + CHANNEL_NAME_SIZE + dataEntrySize );
        fracSecTS = _fracSecTS;
        
        // add in the additional field
        if(fracSecTS){
          registerSize += 12;
          packetSize += TIMESTAMP_SIZE + 9;
        }
      }

    // ready special packet in the buffer to send to sever to announce the data format
    // then call send function to send it
    // returns length of packet in bytes
    uint8_t registerIntStream();
    uint8_t registerFloatStream();
    // ready the data packet in the buffer
    // returns length of packet in bytes
    uint8_t preparePacket( uint32_t timestamp, int16_t* data );
    uint8_t preparePacket( uint32_t timestamp, uint32_t fsTS, int16_t* data );
    uint8_t preparePacket( uint32_t timestamp, float* data );
    uint8_t preparePacket( uint32_t timestamp, uint32_t fsTS, float* data );

  private:
    const uint8_t channels; // number of data points to send
    char * const streamName;  // stream designator with extra common json stuff
    const uint8_t streamNameSize;     // length of stream name
    const uint8_t dataEntrySize;  // size of each entry (max int to decimal places)
    char * const buffer;  // pointer to start of buffer (4 bytes after start of zmq buffer)

    // packet size
    uint8_t fracSecTS;
    uint8_t registerSize;   // regsiter stream packet size in bytes
    uint8_t packetSize;     // data packet size in bytes

    // convert from int16 to char array with space padding
    // pass pointer to the location in the buffer where data starts and fill it
    void int2charArray(int16_t x, char* data);
    void int2charArray(uint32_t x, char* data);

    uint8_t addChannelStr( uint8_t ch, char* buf );
    uint8_t addPreString();

    // add the channel register sequence to the buffer
    // pass pointer to the location in the buffer where data starts and fill it
    void registerChannel( uint8_t ch, char* buf, uint8_t type );

    void addChannelData( uint8_t ch, int16_t x, char* buf );
    void addChannelData( uint8_t ch, float x, char* buf );


    uint8_t regFracSecTS( char* buf );
    uint8_t addFracSecTS( char* buf, uint32_t fsTS);
};

#endif
