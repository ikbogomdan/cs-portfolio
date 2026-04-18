from selenium.webdriver.common.by import By
import sys
import parseclass as pc_



url_name__ = 'aloyoga'

url_parse_ = 'https://www.aloyoga.com/collections/womens-navy'
if str(sys.argv[1]) == 'New Arrivals':
    url_parse_ = 'https://www.aloyoga.com/collections/new-arrivals'

if str(sys.argv[1]) == 'Best sellers':
    url_parse_ = 'https://www.aloyoga.com/collections/bestsellers'

if str(sys.argv[1]) == 'womens-navy':
    url_parse_ = 'https://www.aloyoga.com/collections/womens-navy'

if str(sys.argv[1]) == 'seashell-blue':
    url_parse_ = 'https://www.aloyoga.com/collections/seashell-blue'

if str(sys.argv[1]) == 'spearmint':
    url_parse_ = 'https://www.aloyoga.com/collections/spearmint'

if str(sys.argv[1]) == 'candy-red':
    url_parse_ = 'https://www.aloyoga.com/collections/candy-red'

if str(sys.argv[1]) == 'white':
    url_parse_ = 'https://www.aloyoga.com/collections/white'

if str(sys.argv[1]) == 'taupe':
    url_parse_ = 'https://www.aloyoga.com/collections/taupe'



count_positions_ = int(sys.argv[2])

website_ = pc_.ParseClass(url_parse_, count_positions_, url_name__)
website_.scroll_()
website_.get_elem_(By.CLASS_NAME, 'PlpTile')
website_.get_all_elem(By.CSS_SELECTOR, 'div.product__images', By.CSS_SELECTOR,
               'div.product__images div.image-wrapper img[id*="Image-"]',
               By.CSS_SELECTOR, '.product__data .productTitle.editorial', By.CSS_SELECTOR,
               '.product__data .Price', 'jpeg')
website_.write()

