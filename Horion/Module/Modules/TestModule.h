#pragma once

#include "Module.h"

class TestModule : public IModule {
private:
	int delay = 0;

public:
	float float1 = 0;
	int int1 = 0;
	bool bool1 = true;
	SettingEnum enum1 = SettingEnum(this);

	TestModule();
	~TestModule();

	// Inherited via IModule
	virtual const char* getModuleName() override;
	virtual bool isFlashMode() override;
	virtual void onEnable() override;
	virtual void onTick(GameMode* gm) override;
	virtual void onPostRender(MinecraftUIRenderContext* renderCtx) override;
	virtual void onSendPacket(Packet* p) override;
	virtual void onMove(MoveInputHandler* hand) override;
	virtual void onDisable() override;
	void onLevelRender() override;
};
