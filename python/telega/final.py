from aiogram import Bot, Dispatcher, F
from aiogram.filters import Command, CommandStart
from aiogram.types import (
    KeyboardButton,
    Message,
    ReplyKeyboardMarkup,
    ReplyKeyboardRemove,
    InputMediaPhoto,
    FSInputFile,
    InlineKeyboardButton,
    InlineKeyboardMarkup,
    CallbackQuery

)
import asyncio
from aiogram.exceptions import TelegramRetryAfter, TelegramNetworkError
import os
import json
import time
import subprocess
import dotenv

########################################################################################################################
########################################################################################################################
########################################################################################################################
########################################################################################################################

dotenv.load_dotenv()
BOT_TOKEN = os.getenv('TOKEN')

with open('our_data.json', 'r') as f:
    data_load_ = json.load(f)

to_generate = data_load_['set']
to_regular = data_load_['regular']

waiting_for_number = False
waiting_for_regular = False
pricing = False
coach_price = data_load_['coach_price']
current_back = ''

bot = Bot(token=BOT_TOKEN)
dp = Dispatcher()


########################################################################################################################
########################################################################################################################
########################################################################################################################
########################################################################################################################

button_alo = KeyboardButton(text='alo yoga')
button_2 = KeyboardButton(text='необозримое множество сайтов в ближайшем будущем 🦮')
button_on = KeyboardButton(text='On')
button_4 = KeyboardButton(text='Best sellers')
button_5 = KeyboardButton(text='New Arrivals')
button_6 = KeyboardButton(text='By colour')
button_7 = KeyboardButton(text='womens-navy')
button_8 = KeyboardButton(text='seashell-blue')
button_9 = KeyboardButton(text='spearmint')
button_10 = KeyboardButton(text='candy-red')
button_12 = KeyboardButton(text='white')
button_11 = KeyboardButton(text='taupe')
button_coach = KeyboardButton(text='coach')

button_back = KeyboardButton(text='Вернуться назад 🔙')
button_testing = KeyboardButton(text='Показать архив')
keyboard_start = ReplyKeyboardMarkup(
    keyboard=[[button_alo, button_coach, button_on], [button_testing]],
    resize_keyboard=True
)



keyboard_alo_yoga = ReplyKeyboardMarkup(keyboard=[[button_6, button_4, button_5],
                                                  [button_back]], resize_keyboard=True)
keyboard_alo_colour = ReplyKeyboardMarkup(keyboard=[[button_7, button_8, button_9, button_10, button_11, button_12],
                                                    [button_back]], resize_keyboard=True)

button_coach_1 = KeyboardButton(text='Bags')
button_coach_2 = KeyboardButton(text='Wallets')
button_coach_3 = KeyboardButton(text='Wristlets')
button_coach_4 = KeyboardButton(text='Card Cases')
button_coach_5 = KeyboardButton(text='Clothing')
button_coach_6 = KeyboardButton(text='Shoes')
button_coach_7 = KeyboardButton(text='Jewerly And Watches')
button_coach_8 = KeyboardButton(text='Accessories')
button_coach_9 = KeyboardButton(text='Totes And Carryalls')
button_coach_10 = KeyboardButton(text='Shoulder Bags')
button_coach_11 = KeyboardButton(text='Crossbody Bags')
button_coach_12 = KeyboardButton(text='Satchels and Top Handle Bags')

keyboard_coach = ReplyKeyboardMarkup(keyboard=[ [button_coach_1, button_coach_2, button_coach_3],
[button_coach_4, button_coach_5, button_coach_6], [button_coach_7, button_coach_8, button_coach_9],
            [button_coach_10, button_coach_11, button_coach_12], [button_back]])


button_on_1 = KeyboardButton(text='new-arrivals')
button_on_2 = KeyboardButton(text='best-sellers')
button_on_3 = KeyboardButton(text='shoes')
keyboard_on = ReplyKeyboardMarkup(keyboard=[[button_on_1], [button_on_2], [button_on_3], [button_back]])

button_on_4 = KeyboardButton(text='all')
button_on_5 = KeyboardButton(text='running')
button_on_6 = KeyboardButton(text='traning')
button_on_7 = KeyboardButton(text='tennis')
button_on_8 = KeyboardButton(text='hiking')
button_on_9 = KeyboardButton(text='active-life')
button_on_10 = KeyboardButton(text='on_last')

keyboard_on_category = ReplyKeyboardMarkup(keyboard=[[button_on_4], [button_on_5],
[button_on_6], [button_on_7], [button_on_8], [button_on_9], [button_on_10], [button_back]])


button_coach_price_1 = InlineKeyboardButton(
    text='50-100', callback_data="button_1_click"
)
button_coach_price_2 = InlineKeyboardButton(
    text='100-150', callback_data="button_2_click"
)
button_coach_price_3 = InlineKeyboardButton(
    text='150-200', callback_data="button_3_click"
)
button_coach_price_4 = InlineKeyboardButton(
    text='200-250', callback_data="button_4_click"
)
button_coach_price_5 = InlineKeyboardButton(
    text='250-300', callback_data="button_5_click"
)
keyboard_price = InlineKeyboardMarkup(
    inline_keyboard=[[button_coach_price_1], [button_coach_price_2],
                     [button_coach_price_3], [button_coach_price_4], [button_coach_price_5]]
)


########################################################################################################################
########################################################################################################################
########################################################################################################################
########################################################################################################################

def last_parse(find_name_):
    os.chdir("all_parces")
    all_items = os.listdir()
    name = find_name_
    maximus = max(int(t.split('_')[1]) for t in all_items if (t != '.DS_Store') and (t.split('_')[0] == find_name_))
    os.chdir(f'{name}_{maximus}')
    all_items = os.listdir()
    maximus = max(int(t.split('_')[1]) for t in all_items if t != '.DS_Store' and t!='errors.txt')
    media_groups = list()
    for ii in range (0, maximus + 1):
        os.chdir(f'position_{ii}')
        all_items = os.listdir()
        jpg_files = []
        for t in all_items:
            if t.lower().endswith('.jpg') or t.lower().endswith('.jpeg') or t.lower().endswith('.png'):
                jpg_files.append(os.path.abspath(t))
        media_group = list()
        text_ret_ = to_regular
        with open(f'disc.txt', 'r') as file:
            nm__ = file.readline().replace(' ', '').strip().split('-')[1]
            disc__ = file.readline().replace(' ', '').strip().split('-')[1]
            link_ = file.read().strip()
        text_ret_ = text_ret_.replace('_price_', disc__).replace('_name_', nm__) + '\n' + '\n' + link_
        for i, photo_path in enumerate(jpg_files[:10]):
            if i == 0:
                media_group.append(
                    InputMediaPhoto(
                        media=FSInputFile(photo_path),
                        caption=text_ret_
                    )
                )
            else:
                media_group.append(
                    InputMediaPhoto(
                        media=FSInputFile(photo_path)
                    )
                )
        media_groups.append(media_group)
        os.chdir('..')
    os.chdir('..')
    os.chdir('..')
    time.sleep(1)
    return media_groups


async def safe_send_media_group(chat_id, media_group, bot, delay, max_attempts=5):
    for attempt in range(max_attempts):
        try:
            await bot.send_media_group(chat_id=chat_id, media=media_group)
            break
        except TelegramRetryAfter as e:
            wait_time = e.retry_after
            await asyncio.sleep(wait_time)
        except TelegramNetworkError:
            if attempt < max_attempts - 1:
                await asyncio.sleep(delay * 2 ** attempt)
            else:
                raise


########################################################################################################################
########################################################################################################################
########################################################################################################################
########################################################################################################################
########################################################################################################################
########################################################################################################################

coach_messages = ['Bags', 'Wallets', 'Wristlets', 'Card Cases', 'Clothing', 'Shoes', 'Jewerly And Watches',
'Accessories', 'Totes And Carryalls', 'Shoulder Bags',
'Crossbody Bags', 'Satchels and Top Handle Bags']

@dp.message(F.text.in_(coach_messages))
async def process_dog_answer(message: Message):
    command = ["python", "coach_parser.py", message.text, str(to_generate), coach_price]
    subprocess.run(
        command,
        capture_output=True,
        text=True
    )
    media_groups = last_parse('coach')
    for media_group in media_groups:
        await safe_send_media_group(
            chat_id=message.chat.id,
            media_group=media_group,
            bot=message.bot,
            delay=1.5
        )

alo_messages = ['Best sellers', 'womens-navy', 'seashell-blue', 'spearmint',
'candy-red', 'womens-navy', 'white', 'taupe', 'New Arrivals']

@dp.message(F.text.in_(alo_messages))
async def process_dog_answer(message: Message):
    command = ["python", "alo_parser.py", message.text, str(to_generate)]
    subprocess.run(
        command,
        capture_output=True,
        text=True
    )
    media_groups = last_parse('aloyoga')
    for media_group in media_groups:
        await safe_send_media_group(
            chat_id=message.chat.id,
            media_group=media_group,
            bot=message.bot,
            delay=1.5
        )


on_messages = ['all', 'running', 'traning', 'tennis', 'hiking', 'active-life', 'new-arrivals', 'best-sellers']

@dp.message(F.text.in_(on_messages))
async def process_dog_answer(message: Message):
    command = ["python", "on_parser.py", message.text, str(to_generate)]
    subprocess.run(
        command,
        capture_output=True,
        text=True
    )
    media_groups = last_parse('on')
    for media_group in media_groups:
        await safe_send_media_group(
            chat_id=message.chat.id,
            media_group=media_group,
            bot=message.bot,
            delay=1.5
        )

########################################################################################################################
########################################################################################################################
########################################################################################################################

@dp.message(F.text == 'on_last')
async def process_dog_answer(message: Message):
    media_groups = last_parse('on')
    for media_group in media_groups:
        await safe_send_media_group(
            chat_id=message.chat.id,
            media_group=media_group,
            bot=message.bot,
            delay=1.5
        )

@dp.message(CommandStart())
async def process_start_command(message: Message):
    global current_back
    current_back = 'start'
    await message.answer(
        text='Выберите нужный сайт',
        reply_markup=keyboard_start
    )

@dp.message(Command(commands='coach_price'))
async def process_dog_command(message: Message):
    await message.answer(
        text='Задайте цену', reply_markup=keyboard_price
    )

@dp.message(Command(commands='help'))
async def process_dog_answer(message: Message):
    await message.answer(
        text='Для начала генераций напечатайте /start \nДля установки количества генераций - напишите /set и выберите нужное количество'
        '\nДля установки шаблона с описанием товара напишите /regular и задайте выражение следующего типа:'
             ' *** _price_ *** _name_'
             '\nПример: \n_name_ 🤍'
             '\n🏷️ _price_ с доставкой'
             '\nДоставка 3-5 недель до Вас'
             '\nОформить заказ @agentbuyer1'
    )

@dp.message(Command(commands='price'))
async def process_dog_answer(message: Message):
    global pricing
    pricing = True
    await message.answer("Введите два чиселка мамочка")

@dp.message(Command(commands='set'))
async def process_dog_answer(message: Message):
    global waiting_for_number
    waiting_for_number = True
    await message.answer("Введите число от 1 до 20:")

@dp.message(Command(commands='sset'))
async def process_dog_answer(message: Message):
    await message.answer(f"{to_generate}")

@dp.message(Command(commands='regular'))
async def process_dog_answer(message: Message):
    global waiting_for_regular
    waiting_for_regular = True
    await message.answer("Введите выражение нужного вида:")

@dp.message(Command(commands='sregular'))
async def process_dog_answer(message: Message):
    await message.answer(f"{to_regular}")

########################################################################################################################
########################################################################################################################
########################################################################################################################

coach_prices = ['50 100', '100 150', '150 200', '200 250', '250 300']
@dp.callback_query(F.data.in_([
    "button_1_click",
    "button_2_click",
    "button_3_click",
    "button_4_click",
    "button_5_click"
]))

async def process_dog_answer(callback_query: CallbackQuery):
    global coach_price
    button_number = int(callback_query.data.split('_')[1])
    coach_price = coach_prices[button_number - 1]
    await callback_query.answer()
    await callback_query.message.answer(text='GJ')

@dp.message(F.text == 'coach')
async def process_dog_answer(message: Message):
    global current_back
    current_back = 'start_coach'
    await message.answer(
        text='Выберите категорию',
        reply_markup=keyboard_coach
    )

@dp.message(F.text == 'By colour')
async def process_dog_answer(message: Message):
    global current_back
    current_back = 'alo_colour'
    await message.answer(
        text='Выберите цвет',
        reply_markup=keyboard_alo_colour
    )

@dp.message(F.text == 'Вернуться назад 🔙')
async def process_dog_answer(message: Message):
    global current_back
    if current_back.split('_')[0] == 'start':
        await message.answer(
            text='Back',
            reply_markup=keyboard_start
        )

        current_back = 'start'


    elif current_back == 'alo_colour':
        await message.answer(
            text='Back',
            reply_markup=keyboard_alo_yoga
        )
        current_back = 'start_alo'

    elif current_back == 'on_shoes':
        await message.answer(
            text='Back',
            reply_markup=keyboard_on
        )
        current_back = 'start_on'

@dp.message(F.text == 'On')
async def process_dog_answer(message: Message):
    global current_back
    current_back = 'start_on'
    await message.answer(
        text='Выберите категорию',
        reply_markup=keyboard_on
    )

@dp.message(F.text == 'shoes')
async def process_dog_answer(message: Message):
    global current_back
    current_back = 'on_shoes'
    await message.answer(
        text='Выберите категорию обуви',
        reply_markup=keyboard_on_category
    )

@dp.message(F.text == 'alo yoga')
async def process_dog_answer(message: Message):
    global current_back
    current_back = 'start_alo'
    await message.answer(
        text='Выберите категорию',
        reply_markup=keyboard_alo_yoga
    )

@dp.message(F.text == 'Показать архив')
async def process_dog_answer(message: Message):
    with open('archive.json', 'r') as f:
        current_archive = json.load(f)
    await message.answer(
        text=f"{current_archive}",
        reply_markup=keyboard_alo_yoga
    )

@dp.message(F.text == 'необозримое множество сайтов в ближайшем будущем 🦮')
async def process_cucumber_answer(message: Message):
    await message.answer(
        text='Терпения Илюше',
        reply_markup=ReplyKeyboardRemove()
    )

########################################################################################################################
########################################################################################################################
########################################################################################################################

@dp.message()
async def universal_handler(message: Message):
    global waiting_for_number, to_generate, waiting_for_regular, to_regular
    global data_load_
    global pricing
    global coach_price
    if waiting_for_number:
        try:
            number = int(message.text)
            if 1 <= number <= 20:
                data_load_['set'] = number
                with open('our_data.json', 'w') as file:
                    json.dump(data_load_, file, indent=4)
                to_generate = number
                waiting_for_number = False
                await message.answer(f"Число {number} сохранено")
            else:
                await message.answer("Число должно быть от 1 до 20")
        except ValueError:
            await message.answer("Пожалуйста, введите число")
        return
    if pricing:
        coach_price = message.text

    if waiting_for_regular:
        reg = message.text
        if ('_price_' in reg) and ('_name_' in reg):
            to_regular = reg
            waiting_for_regular = False
            data_load_['regular'] = reg
            with open('our_data.json', 'w') as file:
                json.dump(data_load_, file, indent=4)
            await message.answer(f"Выражение сохранено:\n{to_regular}")
        else:
            await message.answer(
                "Ошибка: выражение должно содержать '_price_' и '_name_'\n"
                "Пример корректного ввода:\n"
                "_name_ 🤍\n"
                "🏷️ _price_ с доставкой"
            )
        return

if __name__ == '__main__':
    dp.run_polling(bot)