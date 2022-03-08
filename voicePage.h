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

	// constructors
	struct _voicePage* (*voicePageConstr)(void);
	GList* (*createToneEntries)(GtkWidget* combo);
	void (*monoCheckBoxChecked)(GtkWidget* checkbutton, struct _voicePage*);
	void (*portaCheckBoxChecked)(GtkWidget* checkbutton, struct _voicePage*);
	void (*portaTimeChanged)(GtkWidget* scale, struct _voicePage*);
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
void programSelected(GtkWidget* pListComboBox, GList* toneEntries);
void monoCheckBoxChecked(GtkWidget* checkbutton, voicePage_t* voicePage);
void portaCheckBoxChecked(GtkWidget* checkbutton, voicePage_t* voicePage); 
void portaTimeChanged(GtkWidget* scale, voicePage_t* voicePage);

