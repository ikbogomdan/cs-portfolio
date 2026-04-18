from selenium.webdriver.common.by import By
import sys
import parseclass as pc_


filter_ = sys.argv[3]
url_name__ = 'coach'
url_parse_ = 'https://www.coachoutlet.com/shop/women/view-all'
category__ = sys.argv[1]
to_concate_ = '?filterCategory='
filter_helper_ = f'&pmin={filter_.split(" ")[0]}&pmax={filter_.split(" ")[1]}&index=0'
count_positions_ = int(sys.argv[2])
fnl_url_ = f'{url_parse_}{to_concate_}{category__.strip().replace(" ", "+")}{filter_helper_}'


website_ = pc_.ParseClass(fnl_url_, count_positions_, 'coach')
website_.scroll_()
website_.get_elem_(By.CSS_SELECTOR, '.product-tile')
website_.get_all_elem(By.TAG_NAME, 'h1', By.CSS_SELECTOR,
               'div div div div div div img',
               By.TAG_NAME, 'h1', By.XPATH,
               "//p[contains(text(), '$')]", 'jpeg')
website_.write()