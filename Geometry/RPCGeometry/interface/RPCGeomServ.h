#ifndef RPCGeometry_RPCGeomServ_h
#define RPCGeometry_RPCGeomServ_h

#include <string>
#include <vector>

#include <iosfwd>

class RPCDetId;
class RPCGeomServ{
 protected:
  void streamChamberName(std::ostream & _os) const;
  void streamRollName(std::ostream & _os) const;
  void streamName(std::ostream & _os) const;
 public:
  RPCGeomServ(const RPCDetId& id);
  virtual ~RPCGeomServ();
  virtual std::string shortname();
  virtual std::string name();
  virtual std::string rollname();
  virtual std::string chambername();
  virtual int eta_partition(); 
  virtual int chambernr();
  virtual int segment() const;
  virtual bool inverted();
  virtual bool zpositive();
  virtual bool aclockwise();
  std::vector<int> channelInChip();
 
protected:
  RPCGeomServ();
  
 protected:
  const RPCDetId* _id;
  std::string _n;
  std::string _sn;
  std::string _cn;
  std::string _rn;
  int _t;
  int _cnr;
  bool _z;
  bool _a;

};
#endif
