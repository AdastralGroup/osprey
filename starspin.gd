extends TextureRect


# Declare member variables here. Examples:
# var a = 2
# var b = "text"
var tween

# Called when the node enters the scene tree for the first time.
func _ready():
	tween = get_tree().create_tween()
	tween.tween_property(self,"rect_rotation",rect_rotation+360,10)
	yield(get_tree().create_timer(10),"timeout")
	_ready()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
