#ifndef Output_h
#define Output_h

#include <iostream>
#include <fstream>
#include <string>

// interface classes to give uniform access to text and binary files
class Output {
 public:
  // constructor and destructor
  Output( std::ostream* op ): os( op ) {}
  Output           ( const Output& w ) = delete;
  Output& operator=( const Output& w ) = delete;
  virtual ~Output() { delete os; }
  // output operator for ostream functions (e.g. "endl")
  Output& operator<<( std::ostream& ( *pf )( std::ostream& ) ) {
    outStream() << pf;
    // return *this to allow output operators concatenation
    return *this;
  }
  // output operator for generic objects
  template <class T>
  Output& operator<<( const T& t ) {
    Writer<T> w( t );
    write( w );
    // return *this to allow output operators concatenation
    return *this;
  }
  // cleaner class to filter output instructions not suited
  // for binary files
  class BaseCleaner {
   public:
    BaseCleaner() {}
    BaseCleaner             ( const BaseCleaner& c ) = delete;
    BaseCleaner& operator = ( const BaseCleaner& c ) = delete;
    virtual ~BaseCleaner() {}
    virtual bool write( Output& w ) = 0;
  };
  class Cleaner {
   public:
    typedef std::ostream& ( *F )( std::ostream& );
    template <class T>
    Cleaner( const T& t ): p( new FormatCleaner<T>(  t ) ) {}
    Cleaner( F pf       ): p( new CallCleaner     ( pf ) ) {}
    Cleaner             ( const Cleaner& c ) = delete;
    Cleaner& operator = ( const Cleaner& c ) = delete;
    ~Cleaner() { delete p; }
    BaseCleaner* get() const { return p; }
   private:
    BaseCleaner* p;
  };
  Output& operator<<( const Cleaner& c ) {
    write( *c.get() );
    // return *this to allow output operators concatenation
    return *this;
  }
  // type conversion
  operator bool() { return status; }
 protected:
  // output stream
  std::ostream* os;
  bool status;
  // visitor wrapper base for output operations
  class BaseWriter {
   public:
    BaseWriter() {}
    BaseWriter           ( const BaseWriter& r ) = delete;
    BaseWriter& operator=( const BaseWriter& r ) = delete;
    virtual ~BaseWriter() {}
    // cannot rely on overloading to select required write function,
    // concrete acceptors (i.e. writer) classes not yet defined
    virtual bool writeT( Output* w ) = 0;
    virtual bool writeB( Output* w ) = 0;
  };
 private:
  // Concrete visitor for generic output types
  template <class T>
  class Writer: public BaseWriter {
   public:
    // constructor and output functions
    Writer( const T& t ): p( &t ) {}
    Writer           ( const Writer& r ) = delete;
    Writer& operator=( const Writer& r ) = delete;
    ~Writer() override {}
    bool writeT( Output* w ) override {
      return static_cast<bool>( w->outStream() << *p );
    }
    bool writeB( Output* w ) override {
      return static_cast<bool>( w->outStream().write(
                                   reinterpret_cast<const char*>( p ),
                                   sizeof( *p ) ) );
    }
   private:
    // output object pointer
    const T* p;
  };
  // Concrete cleaner for generic input types
  template <class T>
  class FormatCleaner: public BaseCleaner {
   public:
    FormatCleaner( const T& t ): p( &t ) {}
    FormatCleaner             ( const FormatCleaner& c ) = delete;
    FormatCleaner& operator = ( const FormatCleaner& c ) = delete;
    ~FormatCleaner() override {}
    bool write( Output& w ) override { return w << *p; }
   private:
    const T* p;
  };
  // Concrete cleaner for ostream functions
  class CallCleaner: public BaseCleaner {
   public:
    CallCleaner( Cleaner::F x ): f( x ) {}
    CallCleaner             ( const CallCleaner& c ) = delete;
    CallCleaner& operator = ( const CallCleaner& c ) = delete;
    ~CallCleaner() override {}
    bool write( Output& w ) override { return w << f; }
   private:
    Cleaner::F f;
  };
  // accept (write) function
  virtual void write( BaseWriter& w ) = 0;
  virtual void write( BaseCleaner& c ) = 0;
  std::ostream& outStream() {
    return ( os == nullptr ? std::cout : *os );
  }
};

// concrete writer for text output files
class TxtOutput: public Output {
 public:
  TxtOutput( std::ostream* op ): Output( op ) {}
  TxtOutput           ( const TxtOutput& w ) = delete;
  TxtOutput& operator=( const TxtOutput& w ) = delete;
  ~TxtOutput() override {}
 private:
  void write( BaseWriter& w ) override { status = w.writeT( this ); } 
  void write( BaseCleaner& c ) override { status = c.write( *this ); } 
};

// concrete writer for binary output files
class BinOutput: public Output {
 public:
  BinOutput( std::ostream* op ): Output( op ) {}
  BinOutput           ( const BinOutput& w ) = delete;
  BinOutput& operator=( const BinOutput& w ) = delete;
  ~BinOutput() override {}
 private:
  void write( BaseWriter& w ) override { status = w.writeB( this ); } 
  void write( BaseCleaner& c ) override {} 
};

// writer factory
class OutputFactory {
 public:
  static Output* create( const std::string& name,
                         std::ios::openmode mode = std::ios::out ) {
    std::ostream* os = ( name.empty() ?
                         nullptr :
                         new std::ofstream( name.c_str(), mode ) );
    Output* w;
    return ( ( mode & std::ios::binary ) ?
             w = new BinOutput( os ) :
             w = new TxtOutput( os ) );
  }
};

#endif

