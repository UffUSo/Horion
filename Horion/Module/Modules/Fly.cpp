#include "Fly.h"

Fly::Fly() : IModule('F', Category::MOVEMENT, "Fly to the sky") {
	mode = (*new SettingEnum(this))
			   .addEntry(EnumEntry("Creative", 0))
			   .addEntry(EnumEntry("CubeGlide", 1))
			   .addEntry(EnumEntry("AirStuck", 2))
			   .addEntry(EnumEntry("Jetpack", 3))
			   .addEntry(EnumEntry("Jetpack2", 4))
			   .addEntry(EnumEntry("Motion", 5))
			   .addEntry(EnumEntry("Moonlight", 6));
	registerEnumSetting("Mode", &mode, 0);
	registerFloatSetting("Horizontal Speed", &this->horizontalSpeed, this->horizontalSpeed, 0.1f, 10.f);
	registerFloatSetting("Vertical Speed", &this->verticalSpeed, this->verticalSpeed, 0.1f, 10.f);
}

Fly::~Fly() {
}

const char *Fly::getModuleName() {
	return ("Fly");
}

void Fly::onEnable() {
	switch (mode.selected) {
	case 5:
		g_Data.getLocalPlayer()->setPos((*g_Data.getLocalPlayer()->getPos()).add(vec3_t(0, 1, 0)));
		break;
	}
}

void Fly::onTick(C_GameMode *gm) {
	++gameTick;

	switch (mode.selected) {
	case 0:
		gm->player->canFly = true;
		break;
	case 1: {
		float calcYaw = (gm->player->yaw + 90) * (PI / 180);

		gameTick++;

		vec3_t pos = *g_Data.getLocalPlayer()->getPos();
		pos.y += 1.3f;
		C_MovePlayerPacket a(g_Data.getLocalPlayer(), pos);
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&a);
		pos.y -= 1.3f;
		C_MovePlayerPacket a2(g_Data.getLocalPlayer(), pos);
		g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&a2);

		vec3_t moveVec;
		moveVec.x = cos(calcYaw) * horizontalSpeed;
		moveVec.z = sin(calcYaw) * horizontalSpeed;

		gm->player->lerpMotion(moveVec);

		if (gameTick >= 5) {
			gameTick = 0;
			float yaw = gm->player->yaw * (PI / 180);
			float length = 4.f;

			float x = -sin(yaw) * length;
			float z = cos(yaw) * length;

			gm->player->setPos(pos.add(vec3_t(x, 0.5f, z)));
		}

		break;
	}
	case 2:
		gm->player->velocity = vec3_t(0, 0, 0);
		break;

	case 3: {
		float calcYaw = (gm->player->yaw + 90) * (PI / 180);
		float calcPitch = (gm->player->pitch) * -(PI / 180);

		vec3_t moveVec;
		moveVec.x = cos(calcYaw) * cos(calcPitch) * horizontalSpeed;
		moveVec.y = sin(calcPitch) * horizontalSpeed;
		moveVec.z = sin(calcYaw) * cos(calcPitch) * horizontalSpeed;

		gm->player->lerpMotion(moveVec);

		break;
	}
	case 4: {
		if (gameTick >= 5) {
			float calcYaw = (gm->player->yaw + 90) * (PI / 180);
			float calcPitch = (gm->player->pitch) * -(PI / 180);

			vec3_t pos = *g_Data.getLocalPlayer()->getPos();
			C_MovePlayerPacket a(g_Data.getLocalPlayer(), pos);
			g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&a);
			pos.y += 0.35f;
			a = C_MovePlayerPacket(g_Data.getLocalPlayer(), pos);
			g_Data.getClientInstance()->loopbackPacketSender->sendToServer(&a);

			gm->player->velocity.y = 0.465f;
			vec3_t moveVec;
			moveVec.x = cos(calcYaw) * cos(calcPitch) * horizontalSpeed;
			moveVec.z = sin(calcYaw) * cos(calcPitch) * horizontalSpeed;

			gm->player->velocity.x = moveVec.x;
			gm->player->velocity.z = moveVec.z;

			float teleportX = cos(calcYaw) * cos(calcPitch) * 0.00000005f;
			float teleportZ = sin(calcYaw) * cos(calcPitch) * 0.00000005f;

			pos = *gm->player->getPos();
			g_Data.getLocalPlayer()->setPos(vec3_t(pos.x + teleportX, pos.y - 0.15f, pos.z + teleportZ));

			gm->player->velocity.y -= 0.15f;
			gameTick = 0;
		}

		gm->player->velocity = vec3_t(0, 0, 0);

		break;
	}
	case 5:
	case 6:
		gm->player->velocity = vec3_t(0, 0, 0);
	}
}

void Fly::onDisable() {
	if (g_Data.getLocalPlayer() == nullptr)
		return;

	switch (mode.selected) {
	case 0:
		if (g_Data.getLocalPlayer()->gamemode != 1)
			g_Data.getLocalPlayer()->canFly = false;
		break;
	case 1:
	case 5:
		g_Data.getLocalPlayer()->velocity = vec3_t(0, 0, 0);
	}
}

void Fly::onMove(C_MoveInputHandler *input) {
	C_LocalPlayer *localPlayer = g_Data.getLocalPlayer();
	if (localPlayer == nullptr)
		return;

	bool keyPressed = false;

	switch (mode.selected) {
	case 5: {
		vec3_t *localPlayerPos = localPlayer->getPos();

		float yaw = localPlayer->yaw;
		vec2_t moveVec2d = {input->forwardMovement, -input->sideMovement};
		bool pressed = moveVec2d.magnitude() > 0.01f;

		if (input->isJumping) {
			localPlayer->velocity.y += verticalSpeed;
			localPlayer->fallDistance = 0.f;
		}

		if (input->isSneakDown) {
			localPlayer->velocity.y -= verticalSpeed;
			localPlayer->fallDistance = 0.f;
		}

		if (input->right) {
			yaw += 90.f;

			if (input->forward)
				yaw -= 45.f;
			else if (input->backward)
				yaw += 45.f;
		}

		if (input->left) {
			yaw -= 90.f;

			if (input->forward)
				yaw += 45.f;
			else if (input->backward)
				yaw -= 45.f;
		}

		if (input->backward && !input->left && !input->right)
			yaw += 180.f;

		if (pressed) {
			float calcYaw = (yaw + 90.f) * (PI / 180.f);
			vec3_t moveVec;

			moveVec.x = cos(calcYaw) * horizontalSpeed;
			moveVec.y = localPlayer->velocity.y;
			moveVec.z = sin(calcYaw) * horizontalSpeed;
			localPlayer->lerpMotion(moveVec);
		}

		break;
	}
	case 6: {
		// Moonlight is an AntiCheat by disepi (Zephyr Developer)
		// This Fly/Glide worked on the Hive in the first half year of 2021
		// Idea from Weather Client (dead by now), TurakanFly from BadMan worked similar with less height loss

		C_LocalPlayer *localPlayer = g_Data.getLocalPlayer();

		if (localPlayer->onGround == false) {
			localPlayer->velocity.y = 0;
		}

		C_GameSettingsInput *input = g_Data.getClientInstance()->getGameSettingsInput();

		if (input == nullptr)
			return;

		float yaw = localPlayer->yaw;

		if (GameData::isKeyDown(*input->forwardKey) && GameData::isKeyDown(*input->backKey))
			return;
		else if (GameData::isKeyDown(*input->forwardKey) && GameData::isKeyDown(*input->rightKey) && !GameData::isKeyDown(*input->leftKey)) {
			yaw += 45.f;
			keyPressed = true;
		} else if (GameData::isKeyDown(*input->forwardKey) && GameData::isKeyDown(*input->leftKey) && !GameData::isKeyDown(*input->rightKey)) {
			yaw -= 45.f;
			keyPressed = true;
		} else if (GameData::isKeyDown(*input->backKey) && GameData::isKeyDown(*input->rightKey) && !GameData::isKeyDown(*input->leftKey)) {
			yaw += 135.f;
			keyPressed = true;
		} else if (GameData::isKeyDown(*input->backKey) && GameData::isKeyDown(*input->leftKey) && !GameData::isKeyDown(*input->rightKey)) {
			yaw -= 135.f;
			keyPressed = true;
		} else if (GameData::isKeyDown(*input->forwardKey)) {
			keyPressed = true;
		} else if (GameData::isKeyDown(*input->backKey)) {
			yaw += 180.f;
			keyPressed = true;
		} else if (GameData::isKeyDown(*input->rightKey) && !GameData::isKeyDown(*input->leftKey)) {
			yaw += 90.f;
			keyPressed = true;
		} else if (GameData::isKeyDown(*input->leftKey) && !GameData::isKeyDown(*input->rightKey)) {
			yaw -= 90.f;
			keyPressed = true;
		} else {
			localPlayer->velocity.x = 0.f;
			localPlayer->velocity.z = 0.f;
			keyPressed = false;
		}

		if (yaw >= 180)
			yaw -= 360.f;

		float calcYaw = (yaw + 90) * (PI / 180);
		vec3_t moveVec;
		moveVec.x = cos(calcYaw) * horizontalSpeed;
		moveVec.y = localPlayer->velocity.y;
		moveVec.z = sin(calcYaw) * horizontalSpeed;

		if (keyPressed) {
			localPlayer->lerpMotion(moveVec);
			keyPressed = false;
		}

		if (gameTick > 6) {
			if (localPlayer->onGround == false) {
				vec3_t pos;
				pos.x = localPlayer->getPos()->x;
				pos.y = localPlayer->getPos()->y - 0.025f;
				pos.z = localPlayer->getPos()->z;

				localPlayer->setPos(pos);
			}

			gameTick = 0;
		}
	}
	}
}
