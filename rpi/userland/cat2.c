/*
  cat with read/write instead of fread/fwrite
*/
#include <stdlib.h>

const int buflen=2000000;

int main() {
  int is;
  char buf[buflen];
  while (is=read(0,buf,buflen)) {  // 0=stdin
    if (is!=buflen) {buf[is]='\n'; is++;} // append newline
    write(1,buf,is);            // 1=stdout
    if (is!=buflen) break;
  }
  exit(0);
}
