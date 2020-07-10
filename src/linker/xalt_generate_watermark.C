#include <sys/time.h>
#include <string>
#include <stdio.h>
#include <limits.h>
#include <sys/utsname.h>
#include <unistd.h>

#include "epoch.h"
#include "xalt_config.h"
#include "parseJsonStr.h"
#include "xalt_vendor_note.h"

int main(int argc, char* argv[])
{
  int          i       = 1;
  const char * uuid    = argv[i++];
  const char * syshost = argv[i++];
  const char * fn      = argv[i++];
  std::string  jsonStr = argv[i++];
  const char * version = XALT_VERSION;

  //--------------------------------------------------
  // Convert an empty jsonStr to "{}"
  if (jsonStr.size() == 0)
    jsonStr = "{}";

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
  // Build user, osName, cwd and system;
  char cwd[PATH_MAX+1];
  getcwd(cwd, sizeof(cwd));

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
  std::string host   = u.nodename;

  std::string array[14][2];

  int k = -1;
  ++k; array[k][0] = "<XALT_Version>";        array[k][1] = version;          /*  0 */
  ++k; array[k][0] = "<Build_host>";          array[k][1] = host;             /*  1 */
  ++k; array[k][0] = "<Build_Syshost>";       array[k][1] = syshost;          /*  2 */
  ++k; array[k][0] = "<Build_compiler>";      array[k][1] = compiler;         /*  3 */
  ++k; array[k][0] = "<Build_compilerPath>";  array[k][1] = compilerPath;     /*  4 */
  ++k; array[k][0] = "<Build_OS>";            array[k][1] = osName;           /*  5 */
  ++k; array[k][0] = "<Build_User>";          array[k][1] = user;             /*  6 */
  ++k; array[k][0] = "<Build_UUID>";          array[k][1] = uuid;             /*  7 */
  ++k; array[k][0] = "<Build_Year>";          array[k][1] = year;             /*  8 */
  ++k; array[k][0] = "<Build_date>";          array[k][1] = date;             /*  9 */
  ++k; array[k][0] = "<Build_LOADEDMODULES>"; array[k][1] = loadedmodules;    /* 10 */
  ++k; array[k][0] = "<Build_LMFILES>";       array[k][1] = lmfiles;          /* 11 */
  ++k; array[k][0] = "<Build_CWD>";           array[k][1] = cwd;              /* 12 */
  ++k; array[k][0] = "<Build_Epoch>";         array[k][1] = epochStr;         /* 13 */

  int n = k+1;

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
  fprintf(fp,"\t.file    \"xalt.s\"\n");
  fprintf(fp,"\t.section .note.GNU-stack,\"\",@progbits\n");
  fprintf(fp,"\t.section .xalt\n");

  fprintf(fp,"\t.asciz \"XALT_Link_Info\"\n"); //this is how to find the section in the exec

  // Print cushion
  fprintf(fp,"\n\t.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\n");
  for (int j = 0; j < n; ++j)
    fprintf(fp,"\t.asciz \"%s%%%%%s%%%%\"\n",array[j][0].c_str(), array[j][1].c_str());

  fprintf(fp,"\t.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\n");
  fprintf(fp,"\t.asciz \"XALT_Link_Info_End\"\n");


  //------------------------------------------------------
  // Write 2nd block that can be read by a vendor note
  // See xalt_vendor_note.c for details

  fprintf(fp,"\n");
  fprintf(fp,"# This block is readable by a vendor note.\n");
  fprintf(fp,"\n");
  fprintf(fp,"\t.section .note.GNU-stack,\"\",@progbits\n");
  fprintf(fp,"\t.section .note.xalt.info,\"a\"\n");
  fprintf(fp,"\t.p2align 2\n");
  fprintf(fp,"\t.long 1f - 0f # name size (not including padding\n");
  fprintf(fp,"\t.long 3f - 2f # desc size (not including padding\n");
  fprintf(fp,"\t.long 0x%x\n",XALT_ELF_NOTE_TYPE);
  fprintf(fp,"0:\t.asciz \"XALT\"\n");
  fprintf(fp,"1:\t.p2align 2\n");
  fprintf(fp,"2:\t.byte 0x%02x\n",XALT_STAMP_SUPPORTED_VERSION);
  fprintf(fp,"\t.asciz \"XALT_Link_Info\"\n"); //this is how to find the section in the exec
  for (int j = 0; j < n; ++j)
    fprintf(fp,"\t.asciz \"%s%%%%%s%%%%\"\n",array[j][0].c_str(), array[j][1].c_str());
  fprintf(fp,"\t.byte 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00\n");
  fprintf(fp,"\t.asciz \"XALT_Link_Info_End\"\n");
  fprintf(fp,"3:\t.p2align 2\n");
  
  fclose(fp);

  fprintf(stdout,"%s\n",epochStr);
  free(epochStr);
  return 0;
}
