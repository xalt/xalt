#include "sys/time.h"
#include <string>
#include <stdio.h>
#include <sys/utsname.h>

#include "epoch.h"
#include "xalt_config.h"
#include "parseJsonStr.h"

int main(int argc, char* argv[])
{
  int          i       = 1;
  const char * uuid    = argv[i++];
  const char * syshost = argv[i++];
  const char * fn      = argv[i++];
  std::string  jsonStr = argv[i++];
  const char * version = XALT_VERSION;

  double tt    = epoch();
  time_t clock = (time_t) tt;

  //--------------------------------------------------
  // Build epoch string and year String
  char * epochStr = NULL;
  asprintf(&epochStr, "%.4f",tt);

  struct tm* t = localtime(&clock);
  const int dateSz = 50;
  char date[dateSz];
  strftime(date, dateSz, "%a_%%_%%_%b_%%_%%_%d_%%_%%_%H:%M:%S_%%_%%_%Y", t);
  char year[5];
  strftime(year,5,"%Y",t);
  
  std::string compiler;
  std::string compilerPath;
  Vstring     linklineA;
  parseCompTJsonStr("COMP_T", jsonStr, compiler, compilerPath, linklineA);
  
  //--------------------------------------------------
  // Build user, osName and system;
  struct utsname u;
  uname(&u);

  const char * user = getenv("USER");
  if (user == NULL)
    user = "unknown";

  const char * loadedmodules = getenv("LOADEDMODULES");
  if (loadedmodules == NULL)
    loadedmodules = "";

  const char * lmfiles = getenv("_LMFILES_");
  if (lmfiles == NULL)
    lmfiles = "";

  std::string osName = u.sysname;
  osName.append("_%_%_");
  osName.append(u.release);
  
  std::string system = u.sysname;

  FILE* fp = fopen(fn,"w");

  //--------------------------------------------------
  // Write assembly code
  fprintf(fp,"# This generated assembly code (%s) is free and unencumbered software released into the public domain\n",fn);
  fprintf(fp,"# Anyone is free to copy, modify, publish, use, compile, sell, or\n");
  fprintf(fp,"# distribute this software, either in source code form or as a compiled\n");
  fprintf(fp,"# binary, for any purpose, commercial or non-commercial, and by any\n");
  fprintf(fp,"# means.\n");
  fprintf(fp,"\n");
  fprintf(fp,"# In jurisdictions that recognize copyright laws, the author or authors\n");
  fprintf(fp,"# of this software dedicate any and all copyright interest in the\n");
  fprintf(fp,"# software to the public domain. We make this dedication for the benefit\n");
  fprintf(fp,"# of the public at large and to the detriment of our heirs and\n");
  fprintf(fp,"# successors. We intend this dedication to be an overt act of\n");
  fprintf(fp,"# relinquishment in perpetuity of all present and future rights to this\n");
  fprintf(fp,"# software under copyright law.\n");
  fprintf(fp,"\n");
  fprintf(fp,"# THE SOFTWARE IS PROVIDED \"AS IS\", WITHOUT WARRANTY OF ANY KIND,\n");
  fprintf(fp,"# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF\n");
  fprintf(fp,"# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.\n");
  fprintf(fp,"# IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR\n");
  fprintf(fp,"# OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,\n");
  fprintf(fp,"# ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR\n");
  fprintf(fp,"# OTHER DEALINGS IN THE SOFTWARE.\n");
  fprintf(fp,"\n");
  if (system == "Darwin")
    fprintf(fp,"\t.section .XALT, .xalt\n");
  else
    fprintf(fp,"\t.section .xalt\n");

  fprintf(fp,"\t.asciz \"XALT_Link_Info\"\n"); //this is how to find the section in the exec

  // Print cushion
  fprintf(fp,"\n\t.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\n");
  fprintf(fp,"\t.asciz \"<XALT_Version>%%%%%s%%%%\"\n",version);
  fprintf(fp,"\t.asciz \"<Build.Syshost>%%%%%s%%%%\"\n",syshost);
  fprintf(fp,"\t.asciz \"<Build.compiler>%%%%%s%%%%\"\n",compiler.c_str());
  fprintf(fp,"\t.asciz \"<Build.compilerPath>%%%%%s%%%%\"\n",compilerPath.c_str());
  fprintf(fp,"\t.asciz \"<Build.OS>%%%%%s%%%%\"\n",osName.c_str());
  fprintf(fp,"\t.asciz \"<Build.User>%%%%%s%%%%\"\n",user);
  fprintf(fp,"\t.asciz \"<Build.UUID>%%%%%s%%%%\"\n",uuid);
  fprintf(fp,"\t.asciz \"<Build.Year>%%%%%s%%%%\"\n",year);
  fprintf(fp,"\t.asciz \"<Build.date>%%%%%s%%%%\"\n",date);
  fprintf(fp,"\t.asciz \"<Build.Epoch>%%%%%s%%%%\"\n",epochStr);
  fprintf(fp,"\t.asciz \"<Build.LOADEDMODULES>%%%%%s%%%%\"\n",loadedmodules);
  fprintf(fp,"\t.asciz \"<Build.LMFILES>%%%%%s%%%%\"\n",lmfiles);
  fprintf(fp,"\t.asciz \"<Build.Epoch>%%%%%s%%%%\"\n",epochStr);
  fprintf(fp,"\t.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\n");
  fprintf(fp,"\t.asciz \"XALT_Link_Info_End\"\n");

  fclose(fp);
  
  fprintf(stdout,"%s\n",epochStr);
  free(epochStr);
  return 0;
}
