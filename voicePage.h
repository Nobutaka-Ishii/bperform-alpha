struct _monoInst {
	GtkWidget* checkBox;
	int monoEnabled; // 0:poly, 1:mono
	void (*toggleMono)(struct _monoInst* monoInst);
};
typedef struct _monoInst monoInst_t;

struct _portaInst {
	GtkWidget* checkBox;
	GtkWidget* label;
	GtkWidget* scale;
	guint portaEnabled;
	guint value;
	guint (*getPortaDuration)(struct _portaInst* portaInst);
};
typedef struct _portaInst portaInst_t;

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

	monoInst_t* monoInst;
	portaInst_t* portaInst;

	// constructors
	struct _voicePage* (*voicePageConstr)(void);
	GList* (*createToneEntries)(GtkWidget* combo);
};
typedef struct _voicePage voicePage_t;

voicePage_t* voicePageConstr(void);
GList* createToneEntries(FILE* fp);

struct _eachTone{
	gchar* name;
	guint msb;
	guint lsb;
	guint pc;
};
typedef struct _eachTone eachTone_t;

void programSelected(GtkWidget* pListComboBox, GList* toneEntries);

