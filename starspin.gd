extends TextureRect


# Declare member variables here. Examples:
# var a = 2
# var b = "text"


# Called when the node enters the scene tree for the first time.
func _ready():
	var tween = create_tween().set_loops()
	tween.tween_property(self,"rotation_degrees",rotation+360,10).from_current()


# Called every frame. 'delta' is the elapsed time since the previous frame.
#func _process(delta):
#	pass
