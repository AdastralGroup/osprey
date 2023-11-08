extends Control

var spin_tween: Tween
var s: sutton
# Called when the node enters the scene tree for the first time.
func _ready():
	$Label3.visible_characters = 0
	spin_tween = create_tween().set_loops().set_parallel()
	spin_tween.tween_property($TextureRect2,"rotation_degrees",rotation+360,20).from_current()
	spin_tween.tween_property($TextureRect,"rotation_degrees",rotation+360,20).from_current()
	spin_tween.tween_property($TextureRect3,"rotation_degrees",rotation-360,20).from_current()
	launch()
	


# Called every frame. 'delta' is the elapsed time since the previous frame.
func _process(delta):
	pass



func launch():
	$Label.text += "\nInitialising native module..."
	s = sutton.new()
	$Label.text += "\nC++ Initialised. Loading themes..."
	var game_theme = s.get_game_assets("open_fortress")
	game_theme["id"] = "open_fortress"
	var main = load("res://main.tscn").instantiate()
	add_child(main)
	$Control.position = Vector2(0,349)
	$Control/Main.theme_json["open_fortress"] = game_theme
	$Control/Main._theme_ready()
	$Label.text += "\nAdastral LAUNCHING NOW!"
	do_the_shiny_thing()

	

func do_the_shiny_thing():
	$Label.hide()
	spin_tween.stop()
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
	



