#ifndef CORE_GAME_SHAPE_H
#define CORE_GAME_SHAPE_H

#include "utils/userTypes.h"

bool8 haveSimilarOffsets(const Prefab_St prefab1, const Prefab_St prefab2);
bool8 isShapeClicked(const ActivePrefab_St* const shape);
bool8 isShapeInBound(const ActivePrefab_St* const shape);
bool8 isShapePlaceable(const ActivePrefab_St* const shape);

void setPrefabBoundingBox(Prefab_St* const prefab);

f32Vector2 getShapeTopLeftCorner(const ActivePrefab_St* const shape);
f32Vector2 getShapeCenter(const ActivePrefab_St shape);
f32Vector2 getOffsetCenter(const Prefab_St prefab);
f32Vector2 getIthBlockPosition(const ActivePrefab_St shape, const u8 i);
s8Vector2 mapShapeToBoardPos(const ActivePrefab_St* const shape);

void addPrefabAndVariants(Prefab_St prefab, PrefabBag_St* const prefabsBag);
void handleShape(ActivePrefab_St* const shape);
void shuffleSlots(GameState_St* const game);
void placeShape(const ActivePrefab_St* const shape, Board_St* const board);

void rotatePrefab(Prefab_St* const prefab, u8 rotateBy);
void mirrorPrefab(Prefab_St* const prefab);
void releaseShape(ActivePrefab_St* const shape, Board_St* const board);

void printPrefabInfo(const Prefab_St prefab);

#endif // CORE_GAME_SHAPE_H