#include <gtk/gtk.h>
#define PORTACCNUM 65

struct _voicePage {

	GtkWidget* voicePage;
	GtkWidget* prgListComboBox;
	GtkWidget* pageContents;
	GtkWidget* pageLeft;
	GtkWidget* pageRight;
	GtkWidget* portaCheckBox;
	GtkWidget* portaTimeScale;
	GtkWidget* monoCheckBox;
	GtkWidget* volBox;
	GtkWidget* volLabel;
	GtkWidget* volScale;
	GtkWidget* panBox;
	GtkWidget* panLabel;
	GtkWidget* panScale;
	GtkWidget* attackBox;
	GtkWidget* attackLabel;
	GtkWidget* attackScale;
	GtkWidget* decayBox;
	GtkWidget* decayLabel;
	GtkWidget* decayScale;
	GtkWidget* releaseBox;
	GtkWidget* releaseLabel;
	GtkWidget* releaseScale;
	GtkWidget* revSendBox;
	GtkWidget* revSendScale;
	GtkWidget* revSendLabel;
	GtkWidget* choSendBox;
	GtkWidget* choSendScale;
	GtkWidget* choSendLabel;
	GList* toneEntries;
	gchar* currentProgram;

	int monoEnabled;
	int portaEnabled;
	uint portaTime;
	uint vol;
	uint pan;
	uint attack;
	uint release;
	uint decay;
	uint cho;
	uint rev;

	// constructors
	struct _voicePage* (*voicePageConstr)(void);
	GList* (*createToneEntries)(GtkWidget* combo);

	// methods
	//void (*programSelected)(GtkWidget* pListComboBox, GList* toneEntries);
	void (*programSelected)(GtkWidget* pListComboBox, struct _voicePage*);
	void (*volChanged)(GtkRange* range, struct _voicePage*);
	void (*panChanged)(GtkRange* range, struct _voicePage*);
	void (*attackChanged)(GtkRange* range, struct _voicePage*);
	void (*decayChanged)(GtkRange* range, struct _voicePage*);
	void (*releaseChanged)(GtkRange* range, struct _voicePage*);
	void (*monoCheckBoxChecked)(GtkWidget* checkbutton, struct _voicePage*);
	void (*portaCheckBoxChecked)(GtkWidget* checkbutton, struct _voicePage*);
	void (*portaTimeChanged)(GtkRange* range, struct _voicePage*);
	void (*choSend)(GtkRange* range, struct _voicePage*);
	void (*revSend)(GtkRange* range, struct _voicePage*);
};
typedef struct _voicePage voicePage_t;

struct _eachTone{
	gchar* name;
	guint msb;
	guint lsb;
	guint pc;
};
typedef struct _eachTone eachTone_t;

voicePage_t* voicePageConstr(void);
GList* createToneEntries(FILE* fp);
void programSelected(GtkWidget* pListComboBox, voicePage_t* vpp);
//void programSelected(GtkWidget* pListComboBox, GList* toneEntries);
void volChanged(GtkRange* range, voicePage_t* vpp);
void panChanged(GtkRange* range, voicePage_t* vpp);
void monoCheckBoxChecked(GtkWidget* checkbutton, voicePage_t* voicePage);
void portaCheckBoxChecked(GtkWidget* checkbutton, voicePage_t* voicePage); 
void portaTimeChanged(GtkRange* range, voicePage_t* voicePage);
void revSend(GtkRange* range, voicePage_t* vpp);
void choSend(GtkRange* range, voicePage_t* vpp);
void attackChanged(GtkRange* range, voicePage_t* vpp);
void releaseChanged(GtkRange* range, voicePage_t* vpp);
void decayChanged(GtkRange* range, voicePage_t* vpp);
 
