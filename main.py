import os
from PIL import Image, ImageDraw

player_x, player_y = 115, 155

COORDS = {
    "subzero": (player_x, player_y),
    "scorpion": (220, 155),
    "scorpion_name": (275, 16),
    "subzero_name": (47, 16),
    "health_1": (60, 27),
    "health_2": (260, 27),
    "number_left": (152, 17),
    "number_right": (168, 17),
    "number_center": (160, 17),
    "score_title": (160, 40),
    "command": (160, 100),
    "subzero_down_block": (player_x, player_y + 20),
    "subzero_down_kick": (player_x, player_y + 10),
    "subzero_down_punch": (player_x, player_y + 20),
    "subzero_left_kick": (player_x, player_y + 20),
    "subzero_left_punch": (player_x + 10, player_y),
    "subzero_neutral_block": (player_x, player_y),
    "subzero_neutral_kick": (player_x, player_y),
    "subzero_neutral_punch": (player_x, player_y),
    "subzero_right_kick": (player_x, player_y),
    "subzero_right_punch": (player_x, player_y),
    "subzero_up_block": (player_x, player_y),
    "subzero_up_kick": (player_x, player_y),
    "subzero_up_punch": (player_x + 10, player_y - 10),
    "subzero_fail1_fail1": (player_x, player_y),
    "subzero_fail2_fail2": (player_x, player_y),
    "subzero_fail3_fail3": (player_x, player_y),
}

CMD_LIST = [
    "up_punch",
    "left_punch",
    "right_punch",
    "down_punch",
    "neutral_punch",
    "up_kick",
    "left_kick",
    "right_kick",
    "down_kick",
    "neutral_kick",
    "up_block",
    "down_block",
    "neutral_block",
    "fail1_fail1",
    "fail2_fail2",
    "fail3_fail3",
]


def overlay_images(background_path, overlay_path, output_path, center_x, center_y):
    """
    Function to overlay images on top of each other given x and y coordinates
    :param background_path:
    :param overlay_path:
    :param output_path:
    :param center_x:
    :param center_y:
    :return:
    """
    background = Image.open(background_path).convert("RGBA")
    overlay = Image.open(overlay_path).convert("RGBA")

    overlay_width, overlay_height = overlay.size

    top_left_x = center_x - overlay_width // 2
    top_left_y = center_y - overlay_height // 2

    combined = background.copy()
    combined.paste(overlay, (top_left_x, top_left_y), overlay)

    combined.save(output_path, format="BMP")


def draw_health(score: int, my_image):
    """
    For a specific score, draw red rectangle for enemy health
    :param score:
    :param my_image:
    :return:
    """
    if score == 0:
        return
    new_image = Image.open(my_image).convert("RGBA")

    x_start, y_start = 211, 24
    x_end = x_start + score - 1
    y_end = 29
    shape = [(x_start, y_start), (x_end, y_end)]

    draw = ImageDraw.Draw(new_image)
    draw.rectangle(shape, fill="#ff0000")

    new_image.save(my_image, format="BMP")


def generate_scene_level(score: int, output_path):
    """
    Generate a generic scene for a given score
    :param score:
    :param output_path:
    :return:
    """
    background = "assets/arenas/background1.bmp"

    img = Image.open(background).convert("RGBA")
    img.save(output_path, format="BMP")

    digits = [int(d) for d in str(score)]
    overlay_images(output_path, "assets/game_info/score_title.bmp", output_path, *COORDS["score_title"])
    if score > 9:
        overlay_images(output_path, f"assets/numbers/number{digits[0]}.bmp", output_path, *COORDS["number_left"])
        overlay_images(output_path, f"assets/numbers/number{digits[1]}.bmp", output_path, *COORDS["number_right"])
    else:
        overlay_images(output_path, f"assets/numbers/number{digits[0]}.bmp", output_path, *COORDS["number_center"])

    overlay_images(output_path, "assets/game_info/health.bmp", output_path, *COORDS["health_1"])
    overlay_images(output_path, "assets/game_info/health.bmp", output_path, *COORDS["health_2"])
    draw_health(score, output_path)

    overlay_images(output_path, "assets/game_info/scorpion_name.bmp", output_path, *COORDS["scorpion_name"])
    overlay_images(output_path, "assets/game_info/subzero_name.bmp", output_path, *COORDS["subzero_name"])

    final_image = Image.open(output_path).convert("RGB")
    final_image.save(output_path, format="BMP")


def generate_commands(score: int, output_path, first_command: str, second_command: str):
    """
    Use previously generated generic scene to create command prompt for user.
    :param score: Current game score
    :param output_path: Path to save the generated scene
    :param first_command: First command string
    :param second_command: Second command string
    """
    if first_command in ["fail1", "fail2", "fail3"]:
        return

    background = f"levels/level_{score}.bmp"

    img = Image.open(background).convert("RGBA")
    img.save(output_path, format="BMP")

    overlay_images(output_path, "assets/enemy/scorpion_idle.bmp", output_path, *COORDS["scorpion"])
    overlay_images(output_path, "assets/player/player_idle.bmp", output_path, *COORDS["subzero"])
    overlay_images(output_path, f"assets/game_info/{first_command}_{second_command}.bmp", output_path, *COORDS["command"])

    final_image = Image.open(output_path).convert("RGB")
    final_image.save(output_path, format="BMP")


def generate_results(score: int, output_path, first_command: str, second_command: str):
    """
    Use previously generated generic scene to create result to display.
    :param score: Current game score
    :param output_path: Path to save the generated scene
    :param first_command: First command string
    :param second_command: Second command string
    """
    background = f"levels/level_{score}.bmp"

    img = Image.open(background).convert("RGBA")
    img.save(output_path, format="BMP")

    if first_command in ["fail1", "fail2", "fail3"]:
        if first_command == "fail1":
            overlay_images(output_path, "assets/enemy/scorpion_kick.bmp", output_path, *COORDS["scorpion"])
            overlay_images(output_path, "assets/player/player_hit.bmp", output_path, *COORDS["subzero"])
        elif first_command in ["fail2", "fail3"]:
            overlay_images(output_path, "assets/enemy/scorpion_wins.bmp", output_path, *COORDS["scorpion"])
            overlay_images(output_path, "assets/player/player_dead.bmp", output_path, *COORDS["subzero"])
            if first_command == "fail3":
                overlay_images(output_path, "assets/game_info/game_over.bmp", output_path, *COORDS["command"])
        final_image = Image.open(output_path).convert("RGB")
        final_image.save(output_path, format="BMP")
        return

    overlay_images(output_path, "assets/enemy/scorpion_hit.bmp", output_path, *COORDS["scorpion"])
    overlay_images(output_path, f"assets/player/{second_command}/{first_command}_{second_command}.bmp", output_path, *COORDS[f"subzero_{first_command}_{second_command}"])

    final_image = Image.open(output_path).convert("RGB")
    final_image.save(output_path, format="BMP")


if __name__ == "__main__":
    # Let manual_range = 100 for full generation and manual_override = True to force generation
    manual_override = True
    manual_range = 5

    os.makedirs("levels", exist_ok=True)

    for s in range(manual_range):
        level_output = f"levels/level_{s}.bmp"

        if os.path.exists(level_output) and manual_override is False:
            continue

        print(f"Generating level {s}")
        generate_scene_level(s, level_output)

    print("Level generation complete")

    os.makedirs("commands", exist_ok=True)
    os.makedirs("results", exist_ok=True)

    for s in range(manual_range):
        os.makedirs(f"commands/level_{s}", exist_ok=True)
        os.makedirs(f"results/level_{s}", exist_ok=True)

    for s in range(manual_range):
        for command in CMD_LIST:
            level_output = f"commands/level_{s}/{command}.bmp"

            if os.path.exists(level_output) and manual_override is False:
                continue

            split_command = command.split("_")

            print(f"Generating level {s}: {command}")
            generate_commands(s, level_output, split_command[0], split_command[1])

    print("Command generation complete")

    for s in range(manual_range):
        for command in CMD_LIST:
            level_output = f"results/level_{s}/{command}.bmp"

            if os.path.exists(level_output) and manual_override is False:
                continue

            split_command = command.split("_")

            print(f"Generating level {s}: {command}")
            generate_results(s, level_output, split_command[0], split_command[1])

    print("Result generation complete")