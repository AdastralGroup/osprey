extends Control
var oops = preload("res://oops.tscn")
var panic = preload("res://panic.tscn")

var spin_tween: Tween
var s: binding

const ErrorLevel = {
	INFO = 0,
	WARNING = 1,
	SERIOUS = 2,
	OOPS = 3,
	PANIC = 4
}

func get_config_file():
	if FileAccess.file_exists("user://config.json"):
		var file = FileAccess.open("user://config.json", FileAccess.READ)
		var content = file.get_as_text()
		file.close()
		return content
	else:
		return ""


func set_config_file(data):
	var file = FileAccess.open("user://config.json", FileAccess.WRITE)
	file.store_string(data)
	file.close()

func _ready():
	s = binding.new()
	s.connect("palace_started",init)
	s.connect("error",_on_error)
	s.init_palace()


func launch():
	var main = load("res://main.tscn").instantiate()
	add_child(main)
	move_child($Control,0)
	$Control/Main.s = s
	$Control/Main.ready_after_winter_init()
	$Control.modulate = Color.TRANSPARENT
	transition_main()


func setup():
	transition_setup()


func finish_setup():
	var custom_path = $Setup.path
	write_path_to_config(custom_path)
	var t = create_tween()
	t.tween_property($Camera2D,"position",Vector2(0,0),1)
	await get_tree().create_timer(1).timeout
	init()


func write_path_to_config(path):
	var conf = get_config_file()
	var json
	if conf == "":
		json = JSON.stringify({"default_install_path": path})
	else:
		var config = JSON.parse_string(conf)
		config["default_install_path"] = path
		json = JSON.stringify(config)
	set_config_file(json)


func init():
	var conf = get_config_file()
	if conf == "" or JSON.parse_string(conf) == null:
		call_deferred("setup")
	else:
		call_deferred("free_setup")
		if(s.find_sourcemod_path() == ""):
			get_tree().change_scene_to_file("res://oops.tscn")
		s.init_games()
		call_deferred("launch")


func transition_main():
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


func free_setup():
	$Setup.queue_free()


func transition_setup():
	var t = get_tree().create_tween()
	t.tween_property($Camera2D,"position",Vector2(0,-640),1)
	await get_tree().create_timer(1).timeout


func _on_error(error_level,error_detail):
	if error_level < ErrorLevel.OOPS:
		print(error_detail)
		return
	var z
	if error_level == ErrorLevel.OOPS:
		z = oops.instantiate()
	elif error_level == ErrorLevel.PANIC:
		z = panic.instantiate()
	z.find_child("Label2").set("text",error_detail)
	get_tree().get_root().call_deferred("add_child",z)
	await get_tree().create_timer(1).timeout
	get_tree().paused = true
