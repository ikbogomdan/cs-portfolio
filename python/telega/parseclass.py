import ssl
import time
import undetected_chromedriver as uc
import init_dirs as id_
from selenium.webdriver.common.by import By
from selenium.webdriver import ActionChains, Keys
import requests
from PIL import Image
from io import BytesIO
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.common.exceptions import (TimeoutException, NoSuchElementException,
                                        WebDriverException, StaleElementReferenceException)
import traceback
ssl._create_default_https_context = ssl._create_unverified_context
from webdriver_manager.chrome import ChromeDriverManager
from selenium.webdriver.chrome.service import Service

class ParseClass:

    
    def __init__(self, url, counter_, url_name_, url_filter_ = ''):
        self.url_name_ = url_name_
        self.errors_ = ''
        service = Service(ChromeDriverManager().install())
        self.browser = uc.Chrome(service=service)
        self.our_url_ = url
        self.browser.set_page_load_timeout(30)
        try:
            self.browser.get(f'{url}{url_filter_}')
            WebDriverWait(self.browser, 15).until(
                EC.presence_of_element_located((By.TAG_NAME, 'body'))
            )
            id_.read_from_file_()
            id_.change_path_()
        except TimeoutException:
            self.errors_ += f"Вкладка не загрузилась за 15 секунд ({self.browser.current_url})\n"
        time.sleep(2)
        self.button = None
        self.counter_ = counter_

    def spamcheck(self):
        buttons = self.browser.find_elements(By.CSS_SELECTOR, "[data-click='close']")
        if buttons:
            buttons[0].click()
            time.sleep(0.5)

    def scroll_(self):
        if self.url_name_ == 'on':
            self.spamcheck()
        try:
            height = self.browser.execute_script("return document.body.scrollHeight")
            self.browser.execute_script(f"window.scrollBy(0,{height})")
            time.sleep(0.2)
            self.browser.execute_script("window.scrollBy(0,-200)")
            time.sleep(2)
        except WebDriverException as e:
            self.errors_ += f"Была ошибка при прокрутке ({self.browser.current_url})\n"

    def get_elem_(self, class_by_, find_):
        try:
            links_ = WebDriverWait(self.browser, 20).until(
                EC.presence_of_all_elements_located((class_by_, find_))
            )
        except Exception as e:
            self.errors_ += f"Не удалось найти элементы товаров ({self.browser.current_url})\n"
            links_ = []

        for ind, link in enumerate(links_):
            if ind % 5 == 0:
                time.sleep(0.5)
            if self.counter_ <= ind:
                break
            (ActionChains(self.browser)
             .key_down(Keys.COMMAND)
             .click(link)
             .key_up(Keys.COMMAND)
             .perform())
            time.sleep(.2)
        time.sleep(3)

    def write(self):
        # тут записать все ошибки
        with open(f'errors.txt', 'w') as file:
            file.write(self.errors_)

        id_.back()
        id_.write_to_file_(self.url_name_, self.our_url_ , self.counter_)

    def coach_screen(self):
        try:
            more_btn = WebDriverWait(self.browser, 10).until(
                EC.element_to_be_clickable((By.XPATH, "//p[contains(text(), 'More')]"))
            )
            more_btn.click()
            time.sleep(0.5)
        except (TimeoutException, NoSuchElementException):
            self.errors_ += f'Кнопка "More" не найдена ({self.browser.current_url})\n'
        except WebDriverException as e:
            self.errors_ += f"Ошибка при клике по More ({self.browser.current_url})\n"

        try:
            clrs_ = WebDriverWait(self.browser, 10).until(
                EC.visibility_of_element_located((By.CLASS_NAME, 'color-variants'))
            )
            clrs_.screenshot("all_colors.png")
        except (TimeoutException, NoSuchElementException):
            self.errors_ += f"Блок цветов не найден ({self.browser.current_url})\n"

    def prices(self, price_by_, price_css_):

        if self.url_name_ == 'aloyoga':
            return self.browser.find_element(price_by_, price_css_).find_elements(By.TAG_NAME, 'span')[
                        1].text

        if self.url_name_ == 'coach':
            price_elements = self.browser.find_elements(price_by_, price_css_)
            if price_elements:
                return price_elements[-1].text
            else:
                return "Цена не найдена"

        if self.url_name_ == 'on':
            price_element = self.browser.find_element(price_by_, price_css_)
            return price_element.text


    def coach_pic(self, src):
        return src.replace("desktopThumbnail", "desktopProductZoom").src.replace("desktopProduct", "desktopProductZoom")

    def get_all_elem(self, tab_load_, tab_load_css_, image_by_,
                     image_css_, name_by_, name_css_, price_by_, price_css_, format_):
        all_tabs = self.browser.window_handles
        for jjj, tab in enumerate(all_tabs[1:]):
            if jjj >= self.counter_:
                break
            try:
                self.browser.switch_to.window(tab)
                id_.touch_pos_dir(jjj)
                try:
                    WebDriverWait(self.browser, 15).until(
                        EC.presence_of_element_located((tab_load_, tab_load_css_)))
                except TimeoutException:
                    self.errors_ +=f"Вкладка {jjj + 1} не загрузилась за 15 секунд ({self.browser.current_url})\n"
                    continue
                if self.url_name_ == 'coach':
                    self.coach_screen()
                if self.url_name_ == 'on':
                    self.spamcheck()
                try:
                    all_pic = WebDriverWait(self.browser, 10).until(
                        EC.presence_of_all_elements_located((image_by_, image_css_))
                    )
                except TimeoutException:
                    self.errors_ +=f"Не найдены изображения для товара ({self.browser.current_url})\n"
                    all_pic = []

                for ind, pic in enumerate(all_pic[:6]):
                    try:
                        src = pic.get_attribute('src')
                        if (self.url_name_ == 'coach'):
                            src = src.replace("desktopThumbnail", "desktopProductZoom").replace("desktopProduct", "desktopProductZoom")
                        if not src:
                            continue
                        resp = requests.get(src, timeout=10)
                        resp.raise_for_status()
                        image = Image.open(BytesIO(resp.content))
                        image.save(f'{ind}.{format_}', quality=95)
                        time.sleep(0.1)
                    except (requests.RequestException, IOError, TimeoutException, NoSuchElementException) as e:
                        self.errors_ += f"Ошибка сохранения изображения {str(e)} ({self.browser.current_url})\n"
                try:
                    name_ = self.browser.find_element(name_by_, name_css_).text
                    price_ = self.prices(price_by_, price_css_)
                    with open(f'disc.txt', 'w') as file:
                        file.write(f'позиция - {name_}\nстоимость - {price_}\n{self.browser.current_url}')
                except (NoSuchElementException, IndexError, TimeoutException) as e:
                    self.errors_ += f"Ошибка получения данных о товаре: {str(e)} ({self.browser.current_url})\n"
                id_.back()
                time.sleep(0.5)
            except Exception as e:
                self.errors_ += f"Критическая ошибка при обработке вкладки {str(e)} ({self.browser.current_url})\n"
                continue

