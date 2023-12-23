extends Control
#var oops = preload("res://oops.tscn")

var spin_tween: Tween
var s: sutton
# Called when the node enters the scene tree for the first time.
func _ready():
	s = sutton.new()
	s.connect("palace_started",launch)
	s.init_palace()

# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass


func do_stuff():
	var main = load("res://main.tscn").instantiate()
	add_child(main)
	$Control/Main.s = s
	$Control/Main.ready_after_sutton()
	$Control.position = Vector2(0,349)
	do_the_shiny_thing()
	




func launch():
	if(s.find_sourcemod_path() == ""):
		get_tree().change_scene_to_file("res://oops.tscn")
	s.init_games()
	call_deferred("do_stuff")

func do_the_shiny_thing():
	$Label.hide()
	$Label3.show()
	spin_tween = create_tween().set_loops().set_parallel()
	var tween = create_tween().set_parallel()
	var t = 1.5
	var trans = Tween.TRANS_EXPO
	var ease = Tween.EASE_OUT
	spin_tween.tween_property($Label3,"rotation_degrees",360.0,t).set_trans(Tween.TRANS_SINE).set_ease(Tween.EASE_OUT)
	tween.tween_property($Label3,"visible_characters",8,t).set_trans(Tween.TRANS_SINE).set_ease(ease)
	for x in [$TextureRect,$TextureRect2,$TextureRect3,$TextureRect4,$TextureRect5,$TextureRect6]:
		tween.tween_property(x,"position",$Node2D.position,t).set_trans(trans).set_ease(ease)
		tween.tween_property(x,"size",Vector2(250,250),t).set_trans(trans).set_ease(ease)
		tween.tween_property(x,"pivot_offset",Vector2(125,125),t).set_trans(trans).set_ease(ease)
		spin_tween.tween_property(x,"rotation_degrees",360.0+180,t).set_trans(Tween.TRANS_SINE).set_ease(Tween.EASE_OUT)
		tween.tween_property(x,"modulate",Color.WHITE,t).set_trans(trans).set_ease(Tween.EASE_IN)
	await get_tree().create_timer(t+0.3).timeout
	$TextureRect2.hide()
	$TextureRect3.hide()
	$TextureRect4.hide()
	$TextureRect5.hide()
	$TextureRect6.hide()
	tween = create_tween().set_parallel()
	tween.tween_property($Control,"position",Vector2(0,0),0.5).set_trans(Tween.TRANS_BOUNCE).set_ease(Tween.EASE_OUT)
	



