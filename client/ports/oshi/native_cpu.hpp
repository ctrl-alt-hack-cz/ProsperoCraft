/* SPDX-License-Identifier: GPL-3.0-or-later */
#pragma once
#include <cpuid.h>
#include <cstdint>
#include <cstdio>
#include <cstring>
namespace prospero_cpu {
using Query=bool(*)(unsigned,unsigned,unsigned&,unsigned&,unsigned&,unsigned&);
struct Snapshot {char brand[96]{},vendor[13]{},signature[9]{};unsigned family=0,model=0,stepping=0;int logical=0;bool kernel_count=false,valid=false;};
inline bool cpuid(unsigned leaf,unsigned sub,unsigned& a,unsigned& b,unsigned& c,unsigned& d){return __get_cpuid_count(leaf,sub,&a,&b,&c,&d)!=0;}
inline Snapshot read(Query query,int kernel_logical){
 Snapshot out;unsigned a=0,b=0,c=0,d=0;
 if(!query(0,0,a,b,c,d))return out;
 std::memcpy(out.vendor,&b,4);std::memcpy(out.vendor+4,&d,4);std::memcpy(out.vendor+8,&c,4);
 if(!query(1,0,a,b,c,d))return out;
 unsigned signature=a,base_family=(a>>8)&15;out.family=base_family+(base_family==15?((a>>20)&255):0);out.model=((a>>4)&15)+((base_family==6||base_family==15)?((a>>16)&15)*16:0);out.stepping=a&15;
 std::snprintf(out.signature,sizeof(out.signature),"%08X",signature);int logical=int((b>>16)&255);
 unsigned x=0,y=0,z=0,w=0;
 if(query(0x0b,1,x,y,z,w)&&(y&65535)>0)logical=int(y&65535);
 bool is_amd=std::strcmp(out.vendor,"AuthenticAMD")==0;
 bool is_zen2=is_amd&&out.family==23;
 if(logical<=0&&is_zen2)logical=16;
 char brand[49]{};bool complete=true;
 for(unsigned n=0;n<3;n++){if(!query(0x80000002+n,0,x,y,z,w)){complete=false;break;}std::memcpy(brand+16*n,&x,4);std::memcpy(brand+16*n+4,&y,4);std::memcpy(brand+16*n+8,&z,4);std::memcpy(brand+16*n+12,&w,4);}
 if(complete){int first=0,last=48;while(first<last&&(brand[first]==' '||brand[first]==0))first++;while(last>first&&(brand[last-1]==' '||brand[last-1]==0))last--;bool printable=true;for(int n=first;n<last;n++)if(brand[n]<32||brand[n]>126)printable=false;if(printable&&last>first)std::memcpy(out.brand,brand+first,unsigned(last-first));}
 if(!out.brand[0]){
  if(is_zen2)std::snprintf(out.brand,sizeof(out.brand),"Custom AMD Ryzen Zen 2 8C/16T");
  else std::snprintf(out.brand,sizeof(out.brand),"%s Family %u Model %u",out.vendor,out.family,out.model);
 }
 out.kernel_count=kernel_logical>0&&kernel_logical<=4096;out.logical=out.kernel_count?kernel_logical:logical;
 if(out.logical<=0&&is_zen2)out.logical=16;
 out.valid=out.vendor[0]&&out.logical>0&&out.logical<=4096;return out;
}
}
