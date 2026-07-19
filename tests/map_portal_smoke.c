#include "map.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void Require(bool ok, const char *message) { if (!ok) { fprintf(stderr,"map_portal smoke failed: %s\n",message); exit(1); } }
static bool ReachableAdjacent(const GameMap *map, int sx, int sy, int gx, int gy) {
    int total=map->width*map->height, head=0, tail=0; int *queue=(int*)malloc((size_t)total*sizeof(int));
    unsigned char *seen=(unsigned char*)calloc((size_t)total,1); bool found=false;
    if (!queue||!seen||!Map_IsWalkable(map,sx,sy)) goto done;
    queue[tail++]=sy*map->width+sx; seen[sy*map->width+sx]=1;
    while(head<tail){ int v=queue[head++],x=v%map->width,y=v/map->width; int dx[4]={1,-1,0,0},dy[4]={0,0,1,-1};
      if (abs(x-gx)+abs(y-gy)<=2) { found=true; break; }
      for(int i=0;i<4;i++){int nx=x+dx[i],ny=y+dy[i],n=ny*map->width+nx; if(Map_IsWithinMapBounds(map,nx,ny)&&!seen[n]&&Map_IsWalkable(map,nx,ny)){seen[n]=1;queue[tail++]=n;}}
    }
 done: free(queue); free(seen); return found;
}
static bool ReachableAdjacentRect(const GameMap *map, int sx, int sy, int gx, int gy, int width, int height) {
    int y;
    int x;
    for (y=gy;y<gy+height;y++) for (x=gx;x<gx+width;x++) if (ReachableAdjacent(map,sx,sy,x,y)) return true;
    return false;
}
int main(void){
 MapCatalog catalog={0}; GameMap ship={0},surface={0},target={0}; int sx,sy; int keyAnchors[]={MAP_ANCHOR_COMM_RELAY,MAP_ANCHOR_CRASH_CLUE,MAP_ANCHOR_ENERGY_CORE,MAP_ANCHOR_SIGNAL_TOWER,MAP_ANCHOR_BOSS_PLAYER_ENTRY,MAP_ANCHOR_MONOLITH_A,MAP_ANCHOR_MONOLITH_B,MAP_ANCHOR_MONOLITH_C,MAP_ANCHOR_WEST_ECHO_BASIN,MAP_ANCHOR_SOUTH_FACILITY};
 Require(MapCatalog_Load(&catalog,"maps/map_catalog.json"),"catalog should load"); Require(Map_LoadById(&surface,&catalog,"planet_surface_01"),"formal surface should load");
 Require(Map_LoadById(&ship,&catalog,"ship_interior"),"ship interior should load");
 { const MapPortal *exitPortal=Map_GetPortalById(&ship,"SHIP_AIRLOCK_EXIT");
   Require(exitPortal!=NULL&&exitPortal->requiresFlag[0]=='\0',"airlock exit should be usable without a hidden stage gate");
   Require(exitPortal->gridX==77&&exitPortal->gridY==52&&exitPortal->width==1&&exitPortal->height==1,"interior portal should use one precise interaction cell at the corridor exit");
   Require(Map_FindPortalNear(&ship,exitPortal->gridX-1,exitPortal->gridY)!=NULL,"standing beside the airlock should expose the exit portal");
   Require(Map_GetUnlockById(&ship,"AIRLOCK_DOOR")==NULL&&Map_IsSwampOuterUnlocked(&ship),"split interior should not keep an animated airlock-door unlock");
   Require(Map_GetPropTileAt(&ship,77,52)==TILE_AIRLOCK_DOOR&&Map_GetGroundTileAt(&ship,77,52)==TILE_VOID&&!Map_IsWalkable(&ship,77,52)&&Map_IsWalkable(&ship,76,52),"corridor exit should show one blocking airlock valve while the standing tile remains walkable");
   for(int y=48;y<=50;y++) Require(Map_GetGroundTileAt(&ship,73,y)==TILE_VOID&&Map_GetAreaAtMap(&ship,73,y)==MAP_AREA_UNKNOWN,"former airlock-link floor and area cells should be fully removed");
   for(int i=0;i<ship.resourceSeedCount;i++) Require(!(ship.resourceSeeds[i].gridX==79&&ship.resourceSeeds[i].gridY==52),"former exterior resource should not remain beyond the airlock boundary");
 }
 Require(surface.hasAreaLayer&&surface.hasHazardLayer&&surface.hasDecorLayer&&surface.hasAnchorLayer&&surface.hasResourceLayer&&surface.hasMonsterLayer&&surface.hasLogLayer&&surface.hasRegionLayer&&surface.hasUnlockLayer&&surface.hasPortalLayer&&surface.hasEntityLayer,"all core layers should exist");
 Require(surface.contentVersion==4,"content version should load the solid-landmark rendering revision");
 { const MapEntity *entity=Map_GetEntityById(&surface,"PLAYER_SHIP"); int blockerCount=0; int treeOrRockCount=0;
   Require(entity&&entity->width==6&&entity->height==6&&entity->blocksMovement,"surface ship visual should occupy a blocking 6x6 entity volume");
   for(int y=42;y<=64;y++)for(int x=40;x<=97;x++){TileType prop=Map_GetPropTileAt(&surface,x,y);if(prop==TILE_ENTITY_BLOCKER)blockerCount++;if(prop==TILE_TREE||prop==TILE_ROCK)treeOrRockCount++;Require(Map_GetGroundTileAt(&surface,x,y)==TILE_FOREST_GROUND,"former exterior ship footprint should use ordinary outdoor grass");Require(Map_GetHazardAt(&surface,x,y)==HAZARD_NONE&&Map_GetDecorAt(&surface,x,y)==MAP_DECOR_NONE,"landing safety zone should not retain hazards or ambient clutter");}
   Require(blockerCount==36&&treeOrRockCount==0,"ship landing zone should keep the invisible collision footprint without surrounding trees or rocks");
   for(int y=entity->gridY;y<entity->gridY+entity->height;y++)for(int x=entity->gridX;x<entity->gridX+entity->width;x++)Require(!Map_IsWalkable(&surface,x,y),"every tile covered by the ship should block player movement");
   surface.propTiles[entity->gridY][entity->gridX]=TILE_VOID;
   Require(!Map_IsWalkable(&surface,entity->gridX,entity->gridY)&&Map_IsOpaque(&surface,entity->gridX,entity->gridY),"ship entity volume should still block movement and sight if a runtime tile state removes a blocker cell");
 }
 Require(Map_GetAnchorPosition(&surface,MAP_ANCHOR_SHIP_EXIT_SPAWN,&sx,&sy)&&Map_IsWalkable(&surface,sx,sy)&&Map_GetHazardAt(&surface,sx,sy)==HAZARD_NONE,"ship spawn should be safe");
 for(int i=0;i<surface.monsterSeedCount;i++) Require(abs(surface.monsterSeeds[i].gridX-sx)+abs(surface.monsterSeeds[i].gridY-sy)>8,"monsters should not spawn beside the ship");
 for(int i=0;i<surface.unlockCount;i++) Map_SetUnlockOpen(&surface,surface.unlocks[i].id,true);
 for(size_t i=0;i<sizeof(keyAnchors)/sizeof(keyAnchors[0]);i++){int x,y,originX,originY,width=1,height=1;TileType tile;Require(Map_GetAnchorPosition(&surface,(MapAnchor)keyAnchors[i],&x,&y),"key anchor should exist");tile=Map_GetPropTileAt(&surface,x,y);if(Map_GetMultiTilePropBounds(&surface,tile,x,y,&originX,&originY,&width,&height)){x=originX;y=originY;}Require(ReachableAdjacentRect(&surface,sx,sy,x,y,width,height),"main task landmark should have a reachable standing tile beside its footprint");}
 for(int i=0;i<surface.resourceSeedCount;i++) Require(Map_IsWalkable(&surface,surface.resourceSeeds[i].gridX,surface.resourceSeeds[i].gridY),"resource seed should be reachable on walkable ground");
 for(int i=0;i<surface.logSeedCount;i++) Require(Map_IsWalkable(&surface,surface.logSeeds[i].gridX,surface.logSeeds[i].gridY),"log seed should be on walkable ground");
 for(int x=0;x<surface.width;x++) for(int y=0;y<5;y++){Require(!Map_IsWalkable(&surface,x,y),"north boundary should be naturally sealed");Require(!Map_IsWalkable(&surface,x,surface.height-1-y),"south boundary should be naturally sealed");}
 for(int y=0;y<surface.height;y++) for(int x=0;x<5;x++){Require(!Map_IsWalkable(&surface,x,y),"west boundary should be naturally sealed");Require(!Map_IsWalkable(&surface,surface.width-1-x,y),"east boundary should be naturally sealed");}
 Require(ReachableAdjacent(&surface,sx,sy,94,52)&&ReachableAdjacent(&surface,sx,sy,84,40)&&ReachableAdjacent(&surface,sx,sy,84,66),"landing zone should expose east, north, and south exploration exits");
 for(int i=0;i<surface.portalCount;i++){const MapPortal *p=&surface.portals[i];Require(MapCatalog_Find(&catalog,p->targetMapId)!=NULL,"portal target should be catalogued");Require(Map_LoadById(&target,&catalog,p->targetMapId),"portal target map should load");Require(Map_GetAnchorPositionByName(&target,p->targetAnchor,NULL,NULL),"portal target anchor should exist");Map_Destroy(&target);}
 Map_Destroy(&ship); Map_Destroy(&surface); puts("map_portal smoke ok"); return 0;
}
