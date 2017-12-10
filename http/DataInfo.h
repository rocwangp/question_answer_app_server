#pragma once


#include "StringUtil.h"
#include <unordered_map>

using std::string;
using std::unordered_map;

class Question
{
public:
    Question() {}
    explicit Question(int questionId,               const string& question,
                      const string& questionDetail, const string& date, 
                      const string& userId,         const string& answerIds)
        : questionId_(questionId),
          question_(question),
          questionDetail_(questionDetail),
          date_(date),
          userId_(userId),
          answerIds_(answerIds),
          likeUserIds_("")
    {  }

    ~Question() {}

    void setQuestionId(int questionId) { questionId_ = questionId; }
    void setQuestion(const string& question) { question_ = question; }
    void setQuestionDetail(const string& questionDetail) { questionDetail_ = questionDetail; }
    void setDate(const string& date) { date_ = date; }
    void setUserId(const string& userId) { userId_ = userId; }
    void setAnswerIds(const string& answerIds) { answerIds_ = answerIds; }
    void setLikeUserIds(const string& likeUserIds) { likeUserIds_ = likeUserIds; }

    int questionId() const { return questionId_; }
    string question() const { return question_; }
    string questionDetail() const { return questionDetail_; }
    string date() const { return date_; }
    string userId() const { return userId_; }
    string answerIds() const { return answerIds_;}
    string likeUserIds() const { return likeUserIds_; }

    bool isValid() const
    {
        return !question_.empty();
    }
    unordered_map<string, string> toMap()
    {
        unordered_map<string, string> questionInfo;
        questionInfo.insert(std::make_pair("questionId", StringUtil::toString(questionId_)));
        questionInfo.insert(std::make_pair("question", question_));
        questionInfo.insert(std::make_pair("questionDetail", questionDetail_));
        questionInfo.insert(std::make_pair("date", date_));
        questionInfo.insert(std::make_pair("userId", userId_));
        questionInfo.insert(std::make_pair("answerIds", answerIds_));
        questionInfo.insert(std::make_pair("likeUserIds", likeUserIds_));
        return questionInfo;
    }
private:
    int questionId_;
    string question_;
    string questionDetail_;
    string date_;
    string userId_;
    string answerIds_;
    string likeUserIds_;
};


class Answer
{
public:
    Answer() {}
    explicit Answer(int answerId,           const string& answer,
                    const string& date,     const string& userId,
                    const string& commentIdStr)
        : answerId_(answerId),
          answer_(answer),
          date_(date),
          userId_(userId),
          commentIds_(commentIdStr),
          likeUserIds_("")
    {  }
    ~Answer() {}

    void setAnswerId(int answerId) { answerId_ = answerId; }
    void setAnswer(const string& answer) { answer_= answer; }
    void setDate(const string& date) { date_ = date; }
    void setUserId(const string& userId) { userId_ = userId; }
    void setCommentIds(const string& commentIds) { commentIds_ = commentIds; }
    void setLikeUserIds(const string& likeUserIds) { likeUserIds_ = likeUserIds; }

    int answerId() const { return answerId_; }
    string answer() const { return answer_; }
    string date() const { return date_; }
    string userId() const { return userId_; }
    string commentIdStr() const { return commentIds_; }
    string likeUserIds() const { return likeUserIds_; }
    
    bool isValid() const
    {
        return !answer_.empty();
    }
    unordered_map<string, string> toMap()
    {
        unordered_map<string, string> answerInfo;
        answerInfo.insert(std::make_pair("answerId", StringUtil::toString(answerId_)));
        answerInfo.insert(std::make_pair("answer", answer_));
        answerInfo.insert(std::make_pair("date", date_));
        answerInfo.insert(std::make_pair("userId", userId_));
        answerInfo.insert(std::make_pair("commentId", commentIds_));
        return answerInfo;
    }
private:
    int answerId_;
    string answer_;
    string date_;
    string userId_;
    string commentIds_;
    string likeUserIds_;
};


class Comment
{
public:
    Comment() {}
    explicit Comment(int commentId,           const string& comment,
                     const string& date,      const string& userId)
        : commentId_(commentId),
          comment_(comment),
          date_(date),
          userId_(userId)
    {  }
    ~Comment() {}

    void setCommentId(int commentId) { commentId_ = commentId; }
    void setComment(const string& comment) { comment_ = comment; }
    void setDate(const string& date) { date_ = date; }
    void setUserId(const string& userId) { userId_ = userId; }

    unordered_map<string, string> toMap()
    {
        unordered_map<string, string> commentInfo;
        commentInfo.insert(std::make_pair("commentId", StringUtil::toString(commentId_)));
        commentInfo.insert(std::make_pair("comment", comment_));
        commentInfo.insert(std::make_pair("date", date_));
        commentInfo.insert(std::make_pair("userId", userId_));
        return commentInfo;
    }
private:
    int commentId_;
    string comment_;
    string date_;
    string userId_;
};


class User
{
public:
    User() {}
    explicit User(int userId,               const string& account, 
                  const string& password,   const string& username, 
                  const string& articleIds = "", 
                  const string& collectQuestionIds = "",
                  const string& careQuestionIds = "", 
                  const string& careUserIds = "",
                  const string& fansIds = "",    
                  const string& publishQuestionIds = "",
                  const string& publishAnswerIds = "",
                  const string& publishCommentIds = "",
                  const string& likeAnswerIds= "")
        : userId_(userId),
          account_(account),
          password_(password),
          username_(username),
          articleIds_(articleIds),
          collectQuestionIds_(collectQuestionIds),
          careQuestionIds_(careQuestionIds),
          careUserIds_(careUserIds),
          fansIds_(fansIds),
          publishQuestionIds_(publishQuestionIds),
          publishAnswerIds_(publishAnswerIds),
          publishCommentIds_(publishCommentIds),
          likeAnswerIds_(likeAnswerIds)
    {  }

    ~User() {}

    int userId() const { return userId_; }
    string account() const { return account_; }
    string password() const { return password_; }
    string username() const { return username_; }
    string articleIds() const { return articleIds_; }
    string collectQuestionIds() const { return collectQuestionIds_; }
    string careQuestionIds() const { return careQuestionIds_; }
    string careUserIds() const { return careUserIds_; }
    string fansIds() const { return fansIds_; }
    string publishQuestionIds() const { return publishQuestionIds_; }
    string publishAnswerIds() const { return publishAnswerIds_; }
    string publishCommentIds() const { return publishCommentIds_; }
    string likeAnswerIds() const { return likeAnswerIds_; }

    void setUserId(int userId) { userId_ = userId; }
    void setAccount(const string& account) { account_ = account; }
    void setPassword(const string& password) { password_ = password; }
    void setUsername(const string& username) { username_ = username; }
    void setArticleIds(const string& articleIds) { articleIds_ = articleIds; }
    void setCollectQuestionIds(const string& collectQuestionIds) { collectQuestionIds_ = collectQuestionIds; }
    void setCareQuestionIds(const string& careQuestionIds) { careQuestionIds_ = careQuestionIds; }
    void setFansIds(const string& fansIds) { fansIds_ = fansIds; }
    void setPublishQuestionIds(const string& publishQuestionIds) { publishQuestionIds_ = publishQuestionIds; }
    void setPublishAnswerIds(const string& publishAnswerIds) { publishAnswerIds_ = publishAnswerIds; }
    void setPublishCommentIds(const string& publishCommentIds) { publishCommentIds_ = publishCommentIds; }
    void setLikeAnswerIds(const string& likeAnswerIds) { likeAnswerIds_ = likeAnswerIds; }

    unordered_map<string, string> toMap()
    {
        unordered_map<string, string> userMap;
        userMap.insert(std::make_pair("userId", StringUtil::toString(userId_)));
        userMap.insert(std::make_pair("account", account_));
        userMap.insert(std::make_pair("password", password_));
        userMap.insert(std::make_pair("articleIds", articleIds_));
        userMap.insert(std::make_pair("collectQuestionIds", collectQuestionIds_));
        userMap.insert(std::make_pair("careQuestionIds", careQuestionIds_));
        userMap.insert(std::make_pair("careUserIds", careUserIds_));
        userMap.insert(std::make_pair("fansIds", fansIds_));
        userMap.insert(std::make_pair("publishQuestionIds", publishQuestionIds_));
        userMap.insert(std::make_pair("publishAnswerIds", publishAnswerIds_));
        userMap.insert(std::make_pair("publishCommentIds", publishCommentIds_));
        userMap.insert(std::make_pair("likeAnswerIds", likeAnswerIds_));
        return userMap;
    }
private:
    int userId_;
    string account_;
    string password_;
    string username_;
    string articleIds_;
    string collectQuestionIds_;
    string careQuestionIds_;
    string careUserIds_;
    string fansIds_;
    string publishQuestionIds_;
    string publishAnswerIds_;
    string publishCommentIds_;
    string likeAnswerIds_;
};
