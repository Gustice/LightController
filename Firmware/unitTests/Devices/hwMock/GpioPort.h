#pragma once

class InputPort {
  public:
    InputPort(){ Value = 0; };
    ~InputPort(){};
    
    int Value;
    int ReadPort(void) { return Value; }
};
