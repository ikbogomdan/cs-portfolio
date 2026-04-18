import os
import json
import shutil

data_ = None

def read_from_file_():
    with open('attempt_count.json', 'r') as f:
        global data_
        data_ = json.load(f)

def write_to_file_(url_name__, url__, count__):
    global data_
    if url_name__ not in data_:
        data_[url_name__] = 1
    else:
        data_[url_name__] += 1

    current_name = f"{url_name__}_{data_[url_name__]}"
    try:
        with open('archive.json', 'r') as f:
            current_archive = json.load(f)
    except (FileNotFoundError, json.JSONDecodeError):
        current_archive = {}
    description = f"{url__}\n Количество позиций - {count__}"

    if url_name__ not in current_archive:
        current_archive[url_name__] = [description, current_name]
    else:
        current_archive[url_name__] += [description, current_name]

    with open('archive.json', 'w') as file:
        json.dump(current_archive, file, indent=4)



    shutil.move(f'{os.getcwd()}/temporary', f'{os.getcwd()}/all_parces/{url_name__}_{data_[url_name__]}')
    with open('attempt_count.json', 'w') as file:
        json.dump(data_, file, indent=4)

def change_path_():
    os.mkdir('temporary')
    os.chdir("temporary")

def touch_pos_dir(jjj):
    os.mkdir(f'position_{jjj}')
    os.chdir(f'position_{jjj}')

def back():
    os.chdir('..')