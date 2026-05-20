#ifndef Input_h
#define Input_h

#include <iostream>
#include <fstream>
#include <string>

// interface classes to give uniform access to text and binary files
class Input {
 public:
  // constructor and destructor
  Input( std::istream* ip ): is( ip ) {}
  Input           ( const Input& r ) = delete;
  Input& operator=( const Input& r ) = delete;
  virtual ~Input() { delete is; }
  // input operator for generic objects
  template <class T>
  Input& operator>>( T& x ) {
    // create wrapper and do input operation
    Reader<T> w( x );
    read( w );
    // return *this to allow input operators concatenation
    return *this;
  }
  // type conversion
  operator bool() { return status; }
 protected:
  // input stream and status
  std::istream* is;
  bool status;
  // visitor wrapper base for input operations
  class BaseReader {
   public:
    BaseReader() {}
    BaseReader           ( const BaseReader& r ) = delete;
    BaseReader& operator=( const BaseReader& r ) = delete;
    virtual ~BaseReader() {}
    // cannot rely on overloading to select required read function,
    // concrete acceptors (i.e. reader) classes not yet defined
    virtual bool readT( Input* r ) = 0; // read from text   file
    virtual bool readB( Input* r ) = 0; // read from binary file
  };
 private:
  // Concrete visitor for generic input types
  template <class T>
  class Reader: public BaseReader {
   public:
    // constructor and input functions
    Reader( T& x ): p( &x ) {}
    Reader           ( const Reader& r ) = delete;
    Reader& operator=( const Reader& r ) = delete;
    ~Reader() override {}
    bool readT( Input* r ) override {
      return static_cast<bool>( r->inStream() >> *p );
    }
    bool readB( Input* r ) override {
      return static_cast<bool>( r->inStream().read(
                                   reinterpret_cast<char*>( p ),
                                   sizeof( *p ) ) );
    }
   private:
    // input object pointer
    T* p;
  };
  // accept (read) function
  virtual void read( BaseReader& w ) = 0;
  std::istream& inStream() {
    return ( is == nullptr ? std::cin : *is );
  }
};

// concrete reader for text input files
class TxtInput: public Input {
 public:
  TxtInput( std::istream* ip ): Input( ip ) {}
  TxtInput           ( const TxtInput& r ) = delete;
  TxtInput& operator=( const TxtInput& r ) = delete;
  ~TxtInput() override {}
 private:
  void read( BaseReader& w ) override { status = w.readT( this ); } 
};

// concrete reader for binary input files
class BinInput: public Input {
 public:
  BinInput( std::istream* ip ): Input( ip ) {}
  BinInput           ( const BinInput& r ) = delete;
  BinInput& operator=( const BinInput& r ) = delete;
  ~BinInput() override {}
 private:
  void read( BaseReader& w ) override { status = w.readB( this ); } 
};

// reader factory
class InputFactory {
 public:
  static Input* create( const std::string& name,
                        std::ios::openmode mode = std::ios::in ) {
    std::istream* is = ( name.empty() ?
                         nullptr :
                         new std::ifstream( name.c_str(), mode ) );
    Input* r;
    return ( ( mode & std::ios::binary ) ?
             r = new BinInput( is ) :
             r = new TxtInput( is ) );
  }
};

#endif

