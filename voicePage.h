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

	// page0 entries
	GtkWidget* voicePage;
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

	monoInst_t* monoInst;
	portaInst_t* portaInst;

	struct _voicePage* (*voicePageConstr)(void);
};
typedef struct _voicePage voicePage_t;

voicePage_t* voicePageConstr(void);

