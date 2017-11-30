#!/usr/bin/env python
# coding=utf-8

import requests
import json
import time
import os,sys
from bs4 import BeautifulSoup
from selenium import webdriver
import re
import random

headers = {
    'User-Agent' : "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/62.0.3202.75 Safari/537.36",
    "Referer": "https://www.zhihu.com/",
    "Host": "www.zhihu.com",
}
def login():
    url = "https://www.zhihu.com"
    loginURL = "https://www.zhihu.com/login/email"



    data = {
        "email": "wangpeng29029@163.com",
        "password": "3764819",
        "rememberme": "true",
    }
    global s
    s = requests.session()
    global xsrf
    if os.path.exists("cookiefile"):
        with open("cookiefile") as f:
            cookie = json.load(f)
        s.cookies.update(cookie)
        req1 = s.get(url, headers=headers)
        soup = BeautifulSoup(req1.text, "html.parser")
        xsrf = soup.find("input", {"name": "_xsrf", "type": "hidden"}).get("value")
        # 建立一个zhihu.html文件,用于验证是否登陆成功
        with open("zhihu.html", "w") as f:
            f.write(req1.content)
    else:
        req = s.get(url, headers=headers)
        print req
        
        soup = BeautifulSoup(req.text, "html.parser")
        xsrf = soup.find("input", {"name": "_xsrf", "type": "hidden"}).get("value")

        data["_xsrf"] = xsrf

        timestamp = int(time.time() * 1000)
        captchaURL = "http://www.zhihu.com/captcha.gif?=" + str(timestamp)
        print captchaURL

        with open("zhihucaptcha.gif", "wb") as f:
            captchaREQ = s.get(captchaURL, headers=headers)
            f.write(captchaREQ.content)
        loginCaptcha = raw_input("input captcha:\n").strip()
        data["captcha"] = loginCaptcha
        print data
        loginREQ = s.post(loginURL, headers=headers, data=data)
        print s.cookies.get_dict()
        with open("cookiefile", "wb") as f:
            json.dump(s.cookies.get_dict(), f)



def answer_splider():
    spider_count = 0
    f = open("zhihu_data.txt", "w") 
    base_url = "https://www.zhihu.com"
    ids = ["19550994", "19552706", "19550517", "19550434", "19550564", "19551137", "19591985", "19551915", "19555513", "19553176", "19551388",
            "19551077", "19552266", "19551556", "19554825", "19560170", "19559052", "19562832", "19551404", "19550453", "19550429", "19550874",
            "19553298", "19551557", "19550560", "19550937", "19555457", "19554827", "19556664", "19562906", "19556950", "19575492", "19609455"]
    random.shuffle(ids)
    re_url = r'((ht|f)tps?):\/\/[\w\-]+(\.[\w\-]+)+([\w\-\.,@?^=%&:\/~\+#]*[\w\-\@?^=%&\/~\+#])?'
    for id in ids:
        spider_page = 1;
        url = "https://www.zhihu.com/topic/" + id + "/top-answers?page="
        print url
        while spider_page < 3:
            page_url = url + str(spider_page)
            spider_page += 1
            browser = webdriver.PhantomJS()
            print page_url
            browser.get(page_url)
            print page_url
            soup = BeautifulSoup(browser.page_source, 'lxml')
            for question in soup.findAll(attrs={"class":"question_link"}):
                href = question.attrs['href']
                question_url = base_url + href
                browser.get(question_url)
                question_soup = BeautifulSoup(browser.page_source, 'lxml')

                question_header_title = question_soup.findAll(attrs={"class":"QuestionHeader-title"})
                if question_header_title:
                    a = 1
                else:
                    continue

                question_title = question_header_title[0]
                dr = re.compile(r'<[^>]+>', re.S)
                title = dr.sub('', question_title.text)
                dr = re.compile(r'&lt.*&gt', re.S)
                title = dr.sub('', title)
                print "question title is " + title
                f.write(title)
                f.write("\n")
                f.write("\r")
                f.write("\n")
                question_header_detail = question_soup.findAll(attrs={"class":"QuestionHeader-detail"});
                if(question_header_detail):
                    question_detail = question_header_detail[0].findAll(attrs={"class":"RichText"})
                    if question_detail:
                        f.write(question_detail[0].text)
                f.write("\n\r\n")

                for answer_item in question_soup.findAll(attrs={"class":"List-item"}):
                    answer_detail_list =  answer_item.findAll(attrs={"class":"RichText CopyrightRichText-richText"})
                    dr = re.compile(r'<[^>]+>', re.S)
                    if answer_detail_list:
                        answer_detail = answer_detail_list[0]
                        detail = dr.sub('', answer_detail.text)
                        dr = re.compile(r'&lt;.*&gt;', re.S)
                        detail = dr.sub('', detail)
                        f.write(detail)
                        f.write("\n\r\n")
                        spider_count += 1
                        if(spider_count > 10):
                            time.sleep(1)
                            spider_count -= 10

                f.write("\r\r\n")
    browser.quit()
    f.close()

#     question_title = soup.findAll(attrs={"class":"QuestionHeader-title"})[0]
#     print question_title.text
#     f.write(question_title.text)
#     f.write("\r\n")
#     question_detail = soup.findAll(attrs={"class":"QuestionHeader-detail"})[0].findAll(attrs={"class":"RichText"})[0]
#     print question_detail.text
#     f.write("\r\n")
#     # for answer_item in soup.findAll(attrs={"class":"List-item"}):
#     #     comment_item = answer_item.findAll(attrs={"class":"ContentItem-actions"})[0]
#     #     comment_button = comment_item.findAll(attrs={"class":"Button ContentItem-action Button--plain Button--withIcon Button--withLabel"})[0]
#     #     print comment_button
#     #     comment_button.click

#     # browser.get(url)
#     soup = BeautifulSoup(browser.page_source, 'lxml')
#     for answer_item in soup.findAll(attrs={"class":"List-item"}):
#         answer_detail = answer_item.findAll(attrs={"class":"RichText CopyrightRichText-richText"})[0]
#         print answer_detail.text
#         f.write(answer_detail.text)
#         f.write("\n")
#         f.write("\r\n")
#         # print answer_item.findAll(attrs={"class":"CommentItem"})
#         # for comment_item in answer_item.findAll(attrs={"class":"CommentItem"}):
#         #     print comment_item.text

#     f.write("\r\r\n")
#     f.close()


if __name__ == "__main__":
    reload(sys)
    sys.setdefaultencoding('utf-8')
    # f = open("zhihu_data.txt", "w")

    login()
    answer_splider()
    # url = "https://www.zhihu.com"
    # home_url = url + "/explore"
    # resp = requests.get(home_url, headers=headers)
    # soup = BeautifulSoup(resp.content, 'lxml')
    # count = 0
    # for question_a in soup.find_all('a', {'class' , 'question_link'}):
    #     title = question_a.text.strip()
    #     print 'spider question is ' + title
    #     f.write(title) 
    #     f.write('\r\n')
    #     href = question_a.attrs['href']
    #     pos = href.find("/answer")
    #     question_url = url + href[0:pos]
    #     question_resp = requests.get(question_url, headers=headers)
    #     question_soup = BeautifulSoup(question_resp.content, 'lxml')
    #     for answer in question_soup.find_all('div', {'class', 'RichContent-inner'}):
    #         f.write(answer.text.strip())
    #         f.write('\r\n') 
    #         count += 1

    #     f.write('\r\r\n')
    #     if count > 10:
    #         time.sleep(1)
    #         count = 0
    # f.close();
    
        

    
