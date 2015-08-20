#include "Geometry/RPCGeometry/interface/RPCGeomServ.h"
#include "DataFormats/MuonDetId/interface/RPCDetId.h"
#include <sstream>
#include <iomanip>

void RPCGeomServ::streamChamberName(std::ostream & _os) const
{
    int _station = _id->station();
    int _region = _id->region();
    int _ring = _id->ring();
    int _layer = _id->layer();
    int _sector = _id->sector();
    int _subsector = _id->subsector();

    std::ios_base::fmtflags _flags(_os.flags());
    _os << std::noshowpos;
    char _osfill(_os.fill('0'));

    if (_region == 0)
        {
            _os << "W";
            _os << std::setw(2) << std::setfill('+') << _ring;
            _os << "_RB" << _station;
            if (_station <= 2)
                (_layer == 1) ? _os << "in" : _os << "out";
            else
                {
                    if(_sector == 4 && _station == 4)
                        switch (_subsector)
                            {
                            case 1: _os << "--"; break;
                            case 2: _os << "-"; break;
                            case 3: _os << "+"; break;
                            case 4: _os << "++"; break;
                            }
                    if (_station == 3 || (_sector != 9 && _sector != 11 && _sector != 4 ))
                        _os << (_subsector == 1 ? '-' : '+');
                }
            _os << "_S" << std::setw(2) << std::setfill('0') << _sector;
        }
    else
        {
            _os << "RE";
            _os << std::setw(2) << std::setfill('+') << _station * _region;
            _os << "_R" << _ring;
            _os << "_CH" << std::setw(2) << std::setfill('0') << segment();
        }

    _os.fill(_osfill);
    _os.flags(_flags);
}

void RPCGeomServ::streamRollName(std::ostream & _os) const
{
    int _region = _id->region();
    int _roll = _id->roll();

    streamChamberName(_os);
    if (_region == 0)
        {
            switch (_roll)
                {
                case 1: _os << "_Backward"; break;
                case 3: _os << "_Forward"; break;
                case 2: _os << "_Middle"; break;
                }
        }
    else
        {
            switch (_roll)
                {
                case 1: _os << "_A"; break;
                case 2: _os << "_B"; break;
                case 3: _os << "_C"; break;
                case 4: _os << "_D"; break;
                case 5: _os << "_E"; break;
                }
        }
}


void RPCGeomServ::streamName(std::ostream & _os) const
{
    streamRollName(_os);
}

RPCGeomServ::RPCGeomServ::RPCGeomServ( const RPCDetId& id )
  : _id( &id ),
    _n( "" ),
    _sn( "" ),
    _cn( "" ),
    _t( -99 ),
    _z( true ),
    _a( true )
{}

RPCGeomServ::~RPCGeomServ( void )
{}
 
std::string 
RPCGeomServ::name( void )
{
    if (_n.empty())
        {
            std::stringstream _os;
            streamName(_os);
            _n = _os.str();
        }
    return _n;
}

std::string
RPCGeomServ::chambername()
{
    if (_cn.empty())
        {
            std::stringstream _os;
            streamChamberName(_os);
            _cn = _os.str();
        }
    return _cn;
}

std::string 
RPCGeomServ::shortname( void )
{
  if( _sn.size() < 1 )
  {
    int station = _id->station();
    int region = _id->region();
    int roll = _id->roll();
    int ring = _id->ring();
    int layer = _id->layer();
    int sector = _id->sector();
    int subsector = _id->subsector();

    std::stringstream os;

    if( region == 0 )
    {
      os << "RB" << station;
      if( station <= 2 ){

	(layer == 1 ) ? os << "in" : os << "out";

      }
      else
      {
	if( sector == 4 && station == 4 )
	{
	  if( subsector == 1 )
	  {
	    os << "--";
	  }
	  else if( subsector == 2 )
	  {
	    os << ",-";
	  }
	  else if( subsector == 3 )
	  {
	    os << "+";
	  }
	  else if( subsector == 4 )
	  {
	    os << "++";
	  }
	}
	else
	{
	  if( subsector == 1 )
	    os << ",-";
	  else
	    os << "+";
	}
      }
      if( roll == 1 )
	os << " B";
      else if( roll == 3 )
	os << " F";
      else if( roll == 2 )
	os << " M";
    }
    else
    {
      os << "Ri" << ring << " Su" << subsector;
    }
    _sn = os.str();
  }
  return _sn;
}

// returns a vector with number of channels for each chip in each FEB
std::vector<int>
RPCGeomServ::channelInChip( void )
{
  std::vector<int> chipCh(4,8);//Endcap
  
  if(_id->region()==0){//Barrel
    chipCh.clear();

    int station = _id->station();
  
    if (station<3 && _id->layer()==1){ // i.e. RB1in ||RB2in  
      chipCh.push_back(7);
      chipCh.push_back(8);
    }else if (station == 1 || station == 3){//i.e. RB1out || RB3 
      chipCh.push_back(7);
      chipCh.push_back(7);
    }else if (station == 2){// i.e. RB2out
      chipCh.push_back(6);
      chipCh.push_back(8);
    }else if (_id->sector() == 4 || _id->sector()==10 ||(_id->sector() == 8 &&  _id->subsector()!=1) || (_id->sector() == 12  &&  _id->subsector()==1)){
      chipCh.push_back(6);//i.e. Sector 4 &  10 RB4 and Sector 8 &12 RB4+
      chipCh.push_back(6);
    }else {
      chipCh.push_back(8);
      chipCh.push_back(8);
    }	
  }

  return chipCh;
}


int 
RPCGeomServ::eta_partition()
{
  if (_t<-90){
    if (_id->region() == 0 ){
      if (this->inverted()) {
	_t = 3*(_id->ring())+ (3-_id->roll())-1;
      }else{
	_t = 3*(_id->ring())+ _id->roll()-2;
      }
    }else{
      _t = _id->region() * (3*(3-_id->ring()) + _id->roll() + 7);
    }
  }
  return _t;
} 

int
RPCGeomServ::chambernr()
{

  // Station1
  if( _id->station() ==1) {
    
    // in
    if(_id->layer() ==1) { 
      
      if(_id->roll()==1) 
	_cnr = 1;
      else 
	_cnr = 2;
    }
    //out
    else 
      {
	if(_id->roll()==1) 
	  _cnr = 3;
	else 
	  _cnr = 4;
	
      }
  }


  //Station 2
  if (_id->station()==2) {
   
    //in
    if(_id->layer()==1) {
      
      if(_id->roll()==1)//backward
	_cnr = 5;
      if(_id->roll()==3)//forward
	_cnr=6;
      if(_id->roll()==2)//middle
	_cnr=7;
    }
    //out
    else {
      
      if(_id->roll()==2)
	
	_cnr=7;

      if(_id->roll()==1)
	_cnr=8;
      if(_id->roll()==3)
	_cnr=9;
    
    }
  }
    
  //RB3- RB3+
  if(_id->station()==3)
    {
      if(_id->subsector()==1) {
	
	if(_id->roll()==1)
	  _cnr=10;
	else 
	  _cnr=11;
      }
      else {
	
	if(_id->roll()==1)
	  _cnr=12;
	else
	  _cnr=13;
      }
      
    }

  //RB4
  if(_id->station()==4) {
    
    if (_id->sector()== 4) {
      
      if ( _id->subsector()==2){//RB4-
	
	if(_id->roll()==1)
	  _cnr=14;
	else
	  _cnr=15;
	
      }
      
      if ( _id->subsector()==3){//RB4+
	
	if(_id->roll()==1)
	  _cnr=16;
	else
	  _cnr=17;
	
      }
      
      if ( _id->subsector()==1) {//RB4--
	
	if(_id->roll()==1)
	  _cnr=18;
	else
	  _cnr=19;
      }
      
      if ( _id->subsector()==4){//RB4++
	
	if(_id->roll()==1)
	  _cnr=20;
	else
	  _cnr=21;
	
      }
      
    }  
    
    else 
      
      {
	if(_id->subsector()==1) {
	  
	  if(_id->roll()==1)
	    _cnr=14;
	  else 
	    _cnr=15;
	}
	else {
	  
	  if(_id->roll()==1)
	    _cnr=16;
	  else
	    _cnr=17;
	} 
      } 
  }
  

  // _cnr=10;
  return _cnr;
  
}

int 
RPCGeomServ::segment() const
{
  int nsub = 6;
  int station = _id->station();
  int ring = _id->ring();
  ( ring == 1 && station > 1 ) ? nsub = 3 : nsub = 6;

  return( _id->subsector() + nsub * ( _id->sector() - 1 ));
}

bool
RPCGeomServ::inverted()
{
  // return !(this->zpositive() && this->aclockwise());
  return !(this->zpositive());
}


bool
RPCGeomServ::zpositive()
{
  if (_id->region()==0 && _t<-90 ){
    if (_id->ring()<0){
      _z=false;
    }
    if (_id->ring()==0){
      if (_id->sector() == 1 || _id->sector() == 4 ||
	  _id->sector() == 5 || _id->sector() == 8 ||
	  _id->sector() == 9 || _id->sector() == 12){
	_z=false;
      }
    } 
  }
 
  return _z;
}

bool
RPCGeomServ::aclockwise()
{
  if (_id->region()==0 && _t<-90 ){
    if (_id->ring() > 0){
      if (_id->layer()==2){
	_a=false;
      }
    }else if(_id->ring() <0){
      if (_id->layer()==1){
	_a=false;
      }
    }else if(_id->ring() ==0) {
      if ((_id->sector() == 1 || _id->sector() == 4 ||
	   _id->sector() == 5 || _id->sector() == 8 ||
	   _id->sector() == 9 || _id->sector() == 12) && _id->layer()==1)
	_a=false;
      else if ((_id->sector() == 2 || _id->sector() == 3 ||
		_id->sector() == 6 || _id->sector() == 7 ||
		_id->sector() == 10|| _id->sector() == 11) && _id->layer()==2)
	_a=false;
    }
  }
  return _a;
}

RPCGeomServ::RPCGeomServ() : _id(0), _n(""), _sn(""), _cn(""), _t (-99), _z(false), _a(false)
{} 

std::ostream & operator<<(std::ostream & _os, RPCGeomServ const & _rpcgeomserv)
{
    _rpcgeomserv.streamName(_os);
    return _os;
}
