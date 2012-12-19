#ifndef _APBIO_H_
#define _APBIO_H_

#define REM_BUTT	( 0x1 << 1 )
#define NOTRESET	( 0x1 << 2 )
#define VIB 0x10
#define LED 0x20
#define MUTE 0x40
void BSP_Init( void );
void APB_SetIO( int Value );
void APB_ClrIO( int Value );
void APB_ToggleIO( int Index );
void CommReset( void );
void apbio_reloadexptime( void );
void enablebutton( void );
void disablebutton( void );

#endif /* _APBIO_H_ */
