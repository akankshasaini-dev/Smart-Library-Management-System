#include "Library.h"
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <vector>
#ifdef _WIN32
#define NOMINMAX
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")
typedef SOCKET socket_t;
#define CLOSESOCKET closesocket
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
typedef int socket_t;
#define CLOSESOCKET close
#endif

static std::string lower(std::string s){ for(char& c:s)c=static_cast<char>(std::tolower(static_cast<unsigned char>(c))); return s; }
static std::string jsonEscape(const std::string& s){ std::string o; for(char c:s){ if(c=='"'||c=='\\'){o+='\\';o+=c;} else if(c=='\n')o+="\\n"; else if(c=='\r')o+="\\r"; else if(c=='\t')o+="\\t"; else if(static_cast<unsigned char>(c)>=32)o+=c;} return o; }
static std::string field(const std::string& b,const std::string& key){
  std::string needle="\""+key+"\""; size_t p=b.find(needle); if(p==std::string::npos)return ""; p=b.find(':',p+needle.size()); if(p==std::string::npos)return ""; ++p; while(p<b.size()&&std::isspace(static_cast<unsigned char>(b[p])))++p;
  if(p>=b.size())return ""; if(b[p]=='"'){++p; std::string v; bool esc=false; for(;p<b.size();++p){char c=b[p]; if(esc){ if(c=='n')v+='\n'; else if(c=='r')v+='\r'; else v+=c; esc=false;} else if(c=='\\')esc=true; else if(c=='"')break; else v+=c;} return v; }
  size_t e=p; while(e<b.size()&&b[e]!=','&&b[e]!='}'&&!std::isspace(static_cast<unsigned char>(b[e])))++e; return b.substr(p,e-p);
}
static int intField(const std::string& b,const std::string& k){try{return std::stoi(field(b,k));}catch(...){return 0;}}
static std::string bookJson(const BookPtr& b){ if(!b)return "null"; std::ostringstream o; o<<"{\"id\":"<<b->id<<",\"title\":\""<<jsonEscape(b->title)<<"\",\"author\":\""<<jsonEscape(b->author)<<"\",\"issued\":"<<(b->isIssued?"true":"false")<<",\"studentId\":\""<<jsonEscape(b->studentId)<<"\",\"studentName\":\""<<jsonEscape(b->studentName)<<"\"}"; return o.str(); }
static std::string booksJson(const std::vector<BookPtr>& books){std::string s="["; for(size_t i=0;i<books.size();++i){if(i)s+=",";s+=bookJson(books[i]);} return s+"]";}
static std::string readFile(const std::string& p){std::ifstream f(p,std::ios::binary); if(!f)return "";std::ostringstream s;s<<f.rdbuf();return s.str();}
static std::string mime(const std::string& p){if(p.size()>=5&&p.substr(p.size()-5)==".html")return "text/html; charset=utf-8";if(p.size()>=4&&p.substr(p.size()-4)==".css")return "text/css; charset=utf-8";if(p.size()>=3&&p.substr(p.size()-3)==".js")return "application/javascript; charset=utf-8";if(p.size()>=5&&p.substr(p.size()-5)==".json")return "application/json";return "text/plain; charset=utf-8";}
static void sendResponse(socket_t c,int status,const std::string& type,const std::string& body){std::string reason=status==200?"OK":status==404?"Not Found":status==400?"Bad Request":"Internal Server Error"; std::ostringstream h;h<<"HTTP/1.1 "<<status<<" "<<reason<<"\r\nContent-Type: "<<type<<"\r\nContent-Length: "<<body.size()<<"\r\nAccess-Control-Allow-Origin: *\r\nAccess-Control-Allow-Headers: Content-Type\r\nAccess-Control-Allow-Methods: GET, POST, OPTIONS\r\nConnection: close\r\n\r\n";std::string out=h.str()+body;size_t sent=0;while(sent<out.size()){int n=send(c,out.data()+sent,static_cast<int>(out.size()-sent),0);if(n<=0)break;sent+=static_cast<size_t>(n);}}
static std::string result(bool ok,const std::string& msg){return std::string("{\"success\":")+(ok?"true":"false")+",\"message\":\""+jsonEscape(msg)+"\"}";}
static void handle(socket_t client,Library& lib){
 std::string req;char buf[8192];int n;while((n=recv(client,buf,sizeof(buf),0))>0){req.append(buf,n);size_t h=req.find("\r\n\r\n");if(h!=std::string::npos){size_t cl=0;size_t cp=req.find("Content-Length:");if(cp!=std::string::npos){cp+=15;while(cp<req.size()&&req[cp]==' ')++cp;cl=static_cast<size_t>(std::stoul(req.substr(cp,req.find("\r\n",cp)-cp)));}if(req.size()>=h+4+cl)break;}}
 size_t split=req.find("\r\n\r\n");if(split==std::string::npos){sendResponse(client,400,"application/json",result(false,"Bad request"));return;}std::string head=req.substr(0,split),body=req.substr(split+4);std::istringstream hs(head);std::string method,path;hs>>method>>path;size_t q=path.find('?');if(q!=std::string::npos)path.resize(q);
 if(method=="OPTIONS"){sendResponse(client,200,"text/plain","");return;}
 if(path.rfind("/api/",0)==0){std::string out;bool ok=true,msg;
  if(path=="/api/books"&&method=="GET")out="{\"success\":true,\"books\":"+booksJson(lib.allBooksSorted())+"}";
  else if(path=="/api/stats"){auto all=lib.allBooksSorted();out="{\"total\":"+std::to_string(all.size())+",\"available\":"+std::to_string(lib.availableBooks().size())+",\"issued\":"+std::to_string(lib.issuedBooks().size())+",\"avlHeight\":"+std::to_string(lib.avl().height())+",\"bstHeight\":"+std::to_string(lib.bst().height())+",\"hashBuckets\":"+std::to_string(lib.hash().bucketCount())+",\"hashLoad\":"+std::to_string(lib.hash().loadFactor())+",\"longestChain\":"+std::to_string(lib.hash().longestChain())+",\"rotations\":"+std::to_string(lib.avl().rotationCount())+"}";}
  else if(path=="/api/verify"){std::string m;ok=lib.verifyIntegrity(m);out=result(ok,m);}
  else if(path=="/api/add"){std::string m;ok=lib.addBook(intField(body,"id"),field(body,"title"),field(body,"author"),m);if(ok)lib.saveToFile();out=result(ok,m);}
  else if(path=="/api/update"){std::string m;int id=intField(body,"id"),newId=intField(body,"newId");if(!newId)newId=id;ok=lib.updateBook(id,field(body,"title"),field(body,"author"),newId,m);if(ok)lib.saveToFile();out=result(ok,m);}
  else if(path=="/api/delete"){std::string m;ok=lib.deleteBook(intField(body,"id"),m);if(ok)lib.saveToFile();out=result(ok,m);}
  else if(path=="/api/issue"){std::string m;ok=lib.issueBook(intField(body,"id"),field(body,"studentId"),field(body,"studentName"),m);if(ok)lib.saveToFile();out=result(ok,m);}
  else if(path=="/api/return"){std::string m;ok=lib.returnBook(intField(body,"id"),m);if(ok)lib.saveToFile();out=result(ok,m);}
  else if(path=="/api/search"){int id=intField(body,"id");int hs=0,bs=0,as=0;BookPtr b=lib.findById(id);auto cmp=lib.compareSearch(id);out="{\"success\":true,\"book\":"+bookJson(b)+",\"hashSteps\":"+std::to_string(cmp.hashSteps)+",\"bstSteps\":"+std::to_string(cmp.bstSteps)+",\"avlSteps\":"+std::to_string(cmp.avlSteps)+"}";}
  else {sendResponse(client,404,"application/json",result(false,"Unknown API route"));return;}
  sendResponse(client,200,"application/json",out);return;
 }
 if(method!="GET"){sendResponse(client,400,"text/plain","Only GET is supported for website files");return;}
 if(path=="/")path="/index.html";if(path.find("..")!=std::string::npos){sendResponse(client,400,"text/plain","Invalid path");return;}std::string file=readFile("web"+path);if(file.empty()){sendResponse(client,404,"text/plain","File not found: "+path);return;}sendResponse(client,200,mime(path),file);
}
int main(){
#ifdef _WIN32
 WSADATA w; if(WSAStartup(MAKEWORD(2,2),&w)!=0){std::cerr<<"Winsock startup failed\n";return 1;}
#endif
 Library library("books.txt");std::string report;library.loadFromFile(report);std::cout<<report<<"\n";
 socket_t server=socket(AF_INET,SOCK_STREAM,0);if(server<0){std::cerr<<"Could not create server socket\n";return 1;}int opt=1;setsockopt(server,SOL_SOCKET,SO_REUSEADDR,reinterpret_cast<const char*>(&opt),sizeof(opt));sockaddr_in addr{};addr.sin_family=AF_INET;addr.sin_addr.s_addr=htonl(INADDR_ANY);addr.sin_port=htons(static_cast<unsigned short>(std::getenv("PORT") ? std::stoi(std::getenv("PORT")) : 8080))
 if(bind(server,reinterpret_cast<sockaddr*>(&addr),sizeof(addr))<0||listen(server,10)<0){std::cerr<<"Could not start server on port 8080\n";return 1;}std::cout<<"Smart Library website running at http://localhost:8080\nPress Ctrl+C to stop.\n";
 while(true){sockaddr_in clientAddr{};
#ifdef _WIN32
 int len=sizeof(clientAddr);
#else
 socklen_t len=sizeof(clientAddr);
#endif
 socket_t client=accept(server,reinterpret_cast<sockaddr*>(&clientAddr),&len);if(client<0)continue;handle(client,library);CLOSESOCKET(client);}
 CLOSESOCKET(server);
#ifdef _WIN32
 WSACleanup();
#endif
 return 0;
}
