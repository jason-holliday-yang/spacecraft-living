#!/usr/bin/env python3
import copy, json, math, random
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
SRC=ROOT/'maps/planet_surface_legacy.tmj'
OUT=ROOT/'maps/planet_surface_01.tmj'
W,H=126,104
T=96
SHIP_X,SHIP_Y=76,49
SHIP_W,SHIP_H=6,6
SHIP_PORTAL_X,SHIP_PORTAL_Y=SHIP_X+SHIP_W,51
SHIP_PORTAL_H=2
SHIP_SPAWN_X,SHIP_SPAWN_Y=SHIP_PORTAL_X+1,52
random.seed(1601)
source=json.loads(SRC.read_text())
by_name={layer['name']:layer for layer in source['layers']}

def layer(name, data):
    old=by_name[name]
    return {k:(data if k=='data' else (W if k=='width' else (H if k=='height' else v))) for k,v in old.items()}

def idx(x,y): return y*W+x

def inside(x,y):
    # Irregular inset silhouette: cliff/forest boundary never follows the raw TMJ rectangle.
    left=8 + int(3*math.sin(y*.19)) + (2 if y<18 else 0)
    right=117 + int(2*math.sin(y*.13+1.4)) - (2 if y>88 else 0)
    top=8 + int(2*math.sin(x*.16+.7))
    bottom=96 + int(3*math.sin(x*.11+2.1))
    return left <= x <= right and top <= y <= bottom

ground=[1]*(W*H)      # TILE_VOID gid
props=[0]*(W*H)
area=[0]*(W*H)
hazard=[0]*(W*H)
decor=[0]*(W*H)

for y in range(H):
  for x in range(W):
    if not inside(x,y):
      continue
    # Broad zones, with a safe landing apron beside the ship.
    if 78<=x<=94 and 44<=y<=62:
      g,a=2,101 # base floor / Base
    elif y<=35:
      g,a=6,105 # ruins / Ruins
    elif x>=99:
      g,a=5,104 # deep swamp
    elif x>=86:
      g,a=4,103 # outer swamp
    else:
      g,a=3,102 # forest
    ground[idx(x,y)]=g
    area[idx(x,y)]=a

# Natural, layered boundary: impassable trees/rocks one to three cells before void.
for y in range(H):
  for x in range(W):
    if not inside(x,y): continue
    near=any(not inside(x+dx,y+dy) for dx,dy in ((1,0),(-1,0),(0,1),(0,-1),(2,0),(-2,0),(0,2),(0,-2)))
    if near:
      props[idx(x,y)] = 7 if (x+y)%3 else 8
      decor[idx(x,y)] = 302

# Clear three readable exits and a ring/shortcut around the landing zone.
clear=set()
def corridor(points, radius=2):
  for ax,ay,bx,by in zip([p[0] for p in points[:-1]],[p[1] for p in points[:-1]],
                         [p[0] for p in points[1:]],[p[1] for p in points[1:]]):
    steps=max(abs(bx-ax),abs(by-ay),1)
    for s in range(steps+1):
      x=round(ax+(bx-ax)*s/steps); y=round(ay+(by-ay)*s/steps)
      for dy in range(-radius,radius+1):
        for dx in range(-radius,radius+1): clear.add((x+dx,y+dy))
# East swamp route, north ruins route, south/west forest route, plus loop and shortcut.
corridor([(84,52),(92,52),(101,47),(109,35)],2)
corridor([(84,52),(88,42),(78,34),(67,29),(64,17)],2)
corridor([(84,52),(88,66),(81,85),(58,87),(45,82)],2)
corridor([(84,52),(75,64),(60,68),(52,61),(84,52)],2)
corridor([(92,52),(91,68),(75,64)],1)
for x,y in clear:
  if 0<=x<W and 0<=y<H and inside(x,y): props[idx(x,y)]=0

# Region-specific prop silhouettes, avoiding routes and critical points.
for y in range(12,96):
  for x in range(10,118):
    if not inside(x,y) or (x,y) in clear or props[idx(x,y)]!=0: continue
    chance=0.0; tile=7
    if y<=35: chance=.055; tile=8
    elif x>=99: chance=.075; tile=8
    elif x>=86: chance=.06; tile=7
    else: chance=.045; tile=7
    if random.random()<chance: props[idx(x,y)]=tile

# Risk layers: sparse near ship, swamp/poison in the east, trip hazards in ruins.
for y in range(H):
  for x in range(W):
    if not inside(x,y) or props[idx(x,y)] or (78<=x<=94 and 44<=y<=62): continue
    if x>=101 and random.random()<.38: hazard[idx(x,y)]=204
    elif x>=88 and random.random()<.24: hazard[idx(x,y)]=203
    elif y<=34 and random.random()<.10: hazard[idx(x,y)]=202
    if random.random()<.12:
      decor[idx(x,y)] = 304 if y<=35 else (303 if x>=86 else 302)

# Gated deep-risk fronts. Unlock objects clear these exact Props cells.
for y in range(27,76): props[idx(98,y)]=24 # TILE_DEEP_BARRIER gid
for y in range(50,55): props[idx(98,y)]=24
for x in range(48,81): props[idx(x,36)]=25 # TILE_RUINS_BARRIER gid
for x in range(66,70): props[idx(x,36)]=25
# Preserve clear alternative retreat lane around each gate without bypassing the barrier front.
for x,y in [(84,52),(84,51),(84,53),(93,52),(67,37),(67,38),(99,52),(100,52)]: props[idx(x,y)]=0

# Landmark props tied to anchors/tasks.
def stamp(cx,cy,gid,w=3,h=3):
  for y in range(cy,cy+h):
    for x in range(cx,cx+w):
      if 0<=x<W and 0<=y<H: props[idx(x,y)]=gid
stamp(92,46,18)   # comm relay
stamp(106,34,19,4,4) # crash clue
stamp(108,49,20)  # energy core
stamp(63,16,21)   # signal tower
for x,y in [(53,22),(63,19),(74,23)]: stamp(x,y,22)
LANDMARK_GIDS={18,19,20,21,22}
# Guarantee standable cells outside each landmark rectangle. Anchors sit near
# the visual center, so clearing around an anchor would punch holes into the
# footprint and make the renderer repeat the texture on every surviving tile.
def clear_landmark_perimeter(x,y,w,h):
  candidates=[(x-1,y+h//2),(x+w,y+h//2),(x+w//2,y-1),(x+w//2,y+h)]
  for px,py in candidates:
    if inside(px,py) and props[idx(px,py)] not in LANDMARK_GIDS:
      props[idx(px,py)]=0
      hazard[idx(px,py)]=0
      decor[idx(px,py)]=0
for footprint in [(92,46,3,3),(106,34,4,4),(108,49,3,3),(63,16,3,3),
                  (53,22,3,3),(63,19,3,3),(74,23,3,3)]:
  clear_landmark_perimeter(*footprint)

# Seed tiles remain usable. If a legacy seed falls inside an authored landmark,
# move the seed outside instead of erasing part of the landmark footprint.
reserved_seed_cells=set()
for lname in ['Resources','Logs']:
  for o in by_name[lname].get('objects',[]):
    ps={p['name']:p.get('value') for p in o.get('properties',[])}
    if isinstance(ps.get('gridX'),int) and isinstance(ps.get('gridY'),int):
      reserved_seed_cells.add((ps['gridX'],ps['gridY']))

def move_seed_object(o,x,y):
  o['x']=x*T; o['y']=y*T
  for p in o.get('properties',[]):
    if p.get('name')=='gridX': p['value']=x
    elif p.get('name')=='gridY': p['value']=y

def find_seed_relocation(origin_x,origin_y):
  for radius in range(1,13):
    for dy in range(-radius,radius+1):
      dx=radius-abs(dy)
      for px in ({origin_x-dx,origin_x+dx} if dx else {origin_x}):
        py=origin_y+dy
        if (inside(px,py) and (px,py) not in reserved_seed_cells
            and ground[idx(px,py)]!=1 and props[idx(px,py)]==0
            and hazard[idx(px,py)]==0):
          return px,py
  return origin_x,origin_y

for lname in ['Resources','Logs']:
  for o in by_name[lname].get('objects',[]):
    ps={p['name']:p.get('value') for p in o.get('properties',[])}; x=ps.get('gridX'); y=ps.get('gridY')
    if not isinstance(x,int) or not isinstance(y,int) or not inside(x,y): continue
    if props[idx(x,y)] in LANDMARK_GIDS:
      reserved_seed_cells.discard((x,y))
      new_x,new_y=find_seed_relocation(x,y)
      move_seed_object(o,new_x,new_y)
      reserved_seed_cells.add((new_x,new_y))
      props[idx(new_x,new_y)]=0
      hazard[idx(new_x,new_y)]=0
      decor[idx(new_x,new_y)]=0
    else:
      props[idx(x,y)]=0
for o in by_name['Monsters'].get('objects',[]):
  ps={p['name']:p.get('value') for p in o.get('properties',[])}; x=ps.get('gridX'); y=ps.get('gridY')
  if isinstance(x,int) and isinstance(y,int):
    for dy in (0,1):
      for dx in (0,1):
        if inside(x+dx,y+dy): props[idx(x+dx,y+dy)]=0

# Keep the former oversized ship footprint as open landing ground.
for y in range(42,65):
  for x in range(40,91):
    if inside(x,y):
      ground[idx(x,y)]=3 # TILE_FOREST_GROUND gid; ordinary outdoor grass.
      area[idx(x,y)]=102
      props[idx(x,y)]=0
      hazard[idx(x,y)]=0
      decor[idx(x,y)]=0
# Extend the clean apron to the full Landing Safety Zone. Preserve authored
# facilities and barriers, but remove natural trees/rocks and ambient clutter.
for y in range(42,65):
  for x in range(91,98):
    if inside(x,y):
      ground[idx(x,y)]=3
      area[idx(x,y)]=102
      if props[idx(x,y)] in (7,8): props[idx(x,y)]=0
      hazard[idx(x,y)]=0
      decor[idx(x,y)]=0
# Invisible Props remain as a map-layer collision footprint. The Entity also
# declares blocksMovement so restored map state cannot make the ship passable.
for y in range(SHIP_Y,SHIP_Y+SHIP_H):
  for x in range(SHIP_X,SHIP_X+SHIP_W):
    props[idx(x,y)]=27 # TILE_ENTITY_BLOCKER gid; collision only, never rendered.

# Keep all data objects and stable ordering/source indices, but replace region/unlock layout.
layers=[layer('Ground',ground),layer('Decor',decor),layer('Props',props),layer('Area',area),layer('Hazard',hazard)]
for name in ['Anchors','Resources','Monsters','Logs']:
  copied=copy.deepcopy(by_name[name])
  if name=='Anchors':
    for anchor in copied.get('objects',[]):
      if anchor.get('name')=='SHIP_EXIT_SPAWN':
        anchor['x']=SHIP_SPAWN_X*T; anchor['y']=SHIP_SPAWN_Y*T
        for p in anchor.get('properties',[]):
          if p.get('name')=='gridX': p['value']=SHIP_SPAWN_X
          elif p.get('name')=='gridY': p['value']=SHIP_SPAWN_Y
      elif anchor.get('name')=='SHIP_EXTERIOR_ORIGIN':
        anchor['x']=SHIP_X*T; anchor['y']=SHIP_Y*T
        for p in anchor.get('properties',[]):
          if p.get('name')=='gridX': p['value']=SHIP_X
          elif p.get('name')=='gridY': p['value']=SHIP_Y
  layers.append(copied)

def prop(name,value,typ='int'): return {'name':name,'type':typ,'value':value}
def obj(oid,name,typ,x,y,w,h,properties):
  return {'id':oid,'name':name,'type':typ,'x':x*T,'y':y*T,'width':w*T,'height':h*T,'rotation':0,'visible':True,'properties':properties}
regions=[
 ('Landing Safety Zone',101,76,42,21,23,300),
 ('Canopy Loop',102,48,42,43,32,170),
 ('Echo Basin',102,20,68,39,25,160),
 ('South Survey Facility',105,63,77,31,17,180),
 ('Outer Mire',103,86,37,15,42,190),
 ('Deep Mire',104,99,26,16,52,210),
 ('Ruins Approach',105,48,28,33,11,200),
 ('Monolith Ring',105,45,14,42,20,220),
 ('Signal Tower Plateau',105,58,12,14,13,240),
 ('Northwest Hunt Grounds',106,18,16,22,20,230),
 ('Crash Investigation Ridge',104,102,28,14,18,250),
]
region_objs=[]
for i,(name,a,x,y,w,h,priority) in enumerate(regions,1001):
 region_objs.append(obj(i,name,'region',x,y,w,h,[prop('regionName',name,'string'),prop('priority',priority),prop('area',a-101),prop('gridX',x),prop('gridY',y),prop('gridWidth',w),prop('gridHeight',h)]))
layers.append({'id':100,'name':'Regions','type':'objectgroup','draworder':'topdown','opacity':1,'visible':True,'x':0,'y':0,'objects':region_objs})
unlocks=[
 ('ROPE_BARRIER_A','rope_barrier',95,57,1,1,22),('ROPE_BARRIER_B','rope_barrier',95,58,1,1,22),('ROPE_BARRIER_C','rope_barrier',95,59,1,1,22),
 ('SWAMP_DEEP_GATE','gate',98,50,1,5,23),('RUINS_GATE','gate',66,36,4,1,24)
]
unlock_objs=[]
for i,(name,typ,x,y,w,h,tile) in enumerate(unlocks,1101):
 unlock_objs.append(obj(i,name,'unlock',x,y,w,h,[prop('unlockId',name,'string'),prop('unlockType',typ,'string'),prop('gridX',x),prop('gridY',y),prop('gridWidth',w),prop('gridHeight',h),prop('clearsTile',tile)]))
layers.append({'id':101,'name':'Unlocks','type':'objectgroup','draworder':'topdown','opacity':1,'visible':True,'x':0,'y':0,'objects':unlock_objs})
portal_layer=copy.deepcopy(by_name['Portals'])
for portal in portal_layer.get('objects',[]):
  if portal.get('name')=='SHIP_EXTERIOR_ENTRY':
    portal['x']=SHIP_PORTAL_X*T; portal['y']=SHIP_PORTAL_Y*T
    portal['width']=T; portal['height']=SHIP_PORTAL_H*T
layers.append(portal_layer)
entity_layer=copy.deepcopy(by_name['Entities'])
for entity in entity_layer.get('objects',[]):
  if entity.get('name')=='PLAYER_SHIP':
    entity['x']=SHIP_X*T; entity['y']=SHIP_Y*T
    entity['width']=SHIP_W*T; entity['height']=SHIP_H*T
    movement_prop=next((p for p in entity.get('properties',[]) if p.get('name')=='blocksMovement'),None)
    if movement_prop is None:
      entity.setdefault('properties',[]).append({'name':'blocksMovement','type':'bool','value':True})
    else:
      movement_prop['type']='bool'; movement_prop['value']=True
layers.append(entity_layer)

result={k:v for k,v in source.items() if k!='layers'}
result['layers']=layers
result['properties']=[{'name':'contentVersion','type':'int','value':4},{'name':'designStatus','type':'string','value':'playable_graybox_v4'}]
result['nextlayerid']=102
result['nextobjectid']=1200
OUT.write_text(json.dumps(result,ensure_ascii=False,indent=2)+"\n")
print(f'wrote {OUT} ({W}x{H}, {len(layers)} layers)')
