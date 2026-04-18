from selenium.webdriver.common.by import By
import parseclass as pc_
import sys
import time
url_parse_shoes= 'https://www.on.com/en-us/shop/womens/shoes'
url_parse_arrivals= 'https://www.on.com/en-us/shop/***/womens'

filter_ = str(sys.argv[1])

if filter_ == 'new-arrivals' or filter_=='best-sellers':
    final_url=url_parse_arrivals.replace('***', filter_)
else:
    final_url = url_parse_shoes+f'/{filter_}'
counter_ = int(sys.argv[2])


website_ = pc_.ParseClass(final_url, counter_, 'on')
website_.scroll_()
website_.get_elem_(By.CSS_SELECTOR, '[data-test-id = "productResults"] > li')
website_.get_all_elem(By.TAG_NAME, 'h1',
               By.CSS_SELECTOR, '[data-test-id="productSelectorList"] li img',
               By.TAG_NAME, 'h1',
               By.CSS_SELECTOR,'[data-test-id="productPriceNumber"]', 'png')
website_.write()




