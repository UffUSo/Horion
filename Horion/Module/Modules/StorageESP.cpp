#include "StorageESP.h"

#include "../../DrawUtils.h"

StorageESP::StorageESP() : IModule(0, Category::VISUAL, "ESP for but storage blocks.") {
}

StorageESP::~StorageESP() {
}

const char* StorageESP::getModuleName() {
	return ("StorageESP");
}

void StorageESP::onPreRender(MinecraftUIRenderContext* renderCtx) {
	if (!Game.isInGame() || !GameData::canUseMoveKeys() || Game.getLocalPlayer() == nullptr)
		return;

	auto ourListLock = std::scoped_lock(this->listLock);

	for (const auto& chest : bufferedChestList) {
		auto storageID = Game.getLocalPlayer()->region->getBlock(chest.upper)->blockLegacy->blockId;
		float math = (float)fmax(0.3f, (float)fmin(1.f, 15));
		DrawUtils::setColor(1.f, 1.f, 1.f, math);

		Vec3 blockPos = chest.lower;
		if (blockPos.x < 0)
			blockPos.x -= 1;
		if (blockPos.z < 0)
			blockPos.z -= 1;
		storageID = Game.getLocalPlayer()->region->getBlock(blockPos)->toLegacy()->blockId;

		auto mathVect = Vec3(chest.upper.floor().add(Vec3(1.f, 1.f, 1.f)).sub(chest.upper));
		mathVect.y = floor(mathVect.y);

		if (storageID == 54) DrawUtils::setColor(1.f, 1.f, 1.f, math);                     // Normal Chest
		if (storageID == 146) DrawUtils::setColor(.92f, .14f, .14f, math);                 // Trapped Chest
		if (storageID == 130) DrawUtils::setColor(0.435294f, 0.215686f, 0.631372f, math);  // Ender Chest
		if (storageID == 458) DrawUtils::setColor(0.62f, 0.31f, 0.04f, math);                 // Barrel
		if (storageID == 205) DrawUtils::setColor(.49f, .17f, .95f, math);                 // Undyed Shulker Box
		if (storageID == 218) DrawUtils::setColor(.08f, .91f, .99f, math);                 // Shulker Box

		DrawUtils::drawBox(chest.lower, chest.upper, (float)fmax(0.2f, 1 / (float)fmax(1, Game.getLocalPlayer()->eyePos0.dist(chest.lower))), true);  // Fancy math to give an illusion of good esp
	}
}

void StorageESP::onTick(GameMode* gm) {
	// Swap list
	auto listLock = Game.lockChestList();
	auto& chestList = Game.getChestList();
	auto ourListLock = std::scoped_lock(this->listLock);

	this->bufferedChestList.clear();
	this->bufferedChestList.insert(this->bufferedChestList.end(), chestList.begin(), chestList.end());
	chestList.clear();
}