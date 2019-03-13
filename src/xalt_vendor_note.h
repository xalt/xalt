#ifndef XALT_VENDOR_NOTE_H
#define XALT_VENDOR_NOTE_H

#define XALT_ELF_NOTE_TYPE    (0x746c6158)
#define XALT_STAMP_SUPPORTED_VERSION (0x02)

void xalt_vendor_note(char ** watermark, int xalt_tracing);

#endif //XALT_VENDOR_NOTE_H
