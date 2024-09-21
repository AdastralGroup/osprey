extends Control
var oops = preload("res://oops.tscn")

var spin_tween: Tween
var s: binding


func _ready():
	s = binding.new()
	s.connect("palace_started",init)
	s.init_palace()


func launch():
	var main = load("res://main.tscn").instantiate()
	add_child(main)
	move_child($Control,0)
	$Control/Main.s = s
	$Control/Main.ready_after_winter_init()
	$Control.modulate = Color.TRANSPARENT
	transition()
	

func init():
	if(s.find_sourcemod_path() == ""):
		get_tree().change_scene_to_file("res://oops.tscn")
	s.init_games()
	call_deferred("launch")

func transition():
	$Label.hide()
	spin_tween = create_tween().set_loops().set_parallel()
	var tween = create_tween().set_parallel()
	var t = 1
	var t2 = 0.5
	var trans = Tween.TRANS_EXPO
	var ease = Tween.EASE_OUT
	spin_tween.tween_property($Label3,"rotation_degrees",360,t).set_trans(Tween.TRANS_SINE).set_ease(Tween.EASE_OUT)
	tween.tween_property($Label3,"visible_characters",8,t).set_trans(Tween.TRANS_SINE).set_ease(ease)
	for x in [$TextureRect,$TextureRect2,$TextureRect3,$TextureRect4,$TextureRect5,$TextureRect6]:
		tween.tween_property(x,"position",$Node2D.position,t).set_trans(trans).set_ease(ease)
		tween.tween_property(x,"size",Vector2(200,200),t).set_trans(trans).set_ease(ease)
		tween.tween_property(x,"pivot_offset",Vector2(100,100),t).set_trans(trans).set_ease(ease)
		spin_tween.tween_property(x,"rotation_degrees",360.0+180,t).set_trans(Tween.TRANS_SINE).set_ease(Tween.EASE_OUT)
		tween.tween_property(x,"modulate",Color.WHITE,t).set_trans(trans).set_ease(Tween.EASE_IN)
	$TextureRect.reparent($Control/Main,false)
	await get_tree().create_timer(t-t2).timeout
	tween = create_tween().set_parallel()
	tween.tween_property($Control,"modulate",Color.WHITE,t2).set_trans(Tween.TRANS_LINEAR)
	await get_tree().create_timer(t2).timeout
	$TextureRect2.hide()
	$TextureRect3.hide()
	$TextureRect4.hide()
	$TextureRect5.hide()
	$TextureRect6.hide()
	$Label3.reparent($Control/Main)
	await get_tree().create_timer(0.5).timeout
	
