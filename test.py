import time
import feedparser

start = time.time()

feed = feedparser.parse("./data/employees-big.xml")

end = time.time()
print("Execution time:", end-start, "seconds with feedparser")
