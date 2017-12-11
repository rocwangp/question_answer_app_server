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
    explicit User(int userId,               const string& username, 
                  const string& password,   const string& nickname, 
                  const string& articleIds = "", 
                  const string& questionCollectedIds = "",
                  const string& questionFollowedIds = "", 
                  const string& userFollowedIds = "",
                  const string& fansIds = "",    
                  const string& questionPublishedIds = "",
                  const string& answerPublishedIds = "",
                  const string& commentPublishedIds = "",
                  const string& answerUpvotedIds= "")
        : userId_(userId),
          username_(username),
          password_(password),
          nickname_(nickname),
          articleIds_(articleIds),
          questionCollectedIds_(questionCollectedIds),
          questionFollowedIds_(questionFollowedIds),
          userFollowedIds_(userFollowedIds),
          fansIds_(fansIds),
          questionPublishedIds_(questionPublishedIds),
          answerPublishedIds_(answerPublishedIds),
          commentPublishedIds_(commentPublishedIds),
          answerUpvotedIds_(answerUpvotedIds)
    {  }

    ~User() {}

    int userId() const { return userId_; }
    string username() const { return username_; }
    string password() const { return password_; }
    string nickname() const { return nickname_; }
    string articleIds() const { return articleIds_; }
    string questionCollectedIds() const { return questionCollectedIds_; }
    string questionFollowedIds() const { return questionFollowedIds_; }
    string userFollowedIds() const { return userFollowedIds_; }
    string fansIds() const { return fansIds_; }
    string questionPublishedIds() const { return questionPublishedIds_; }
    string answerPublishedIds() const { return answerPublishedIds_; }
    string commentPublishedIds() const { return commentPublishedIds_; }
    string answerUpvotedIds() const { return answerUpvotedIds_; }

    void setUserId(int userId) { userId_ = userId; }
    void setusername(const string& username) { username_ = username; }
    void setPassword(const string& password) { password_ = password; }
    void setUsername(const string& username) { username_ = username; }
    void setArticleIds(const string& articleIds) { articleIds_ = articleIds; }
    void setquestionCollectedIds(const string& questionCollectedIds) { questionCollectedIds_ = questionCollectedIds; }
    void setquestionFollowedIds(const string& questionFollowedIds) { questionFollowedIds_ = questionFollowedIds; }
    void setFansIds(const string& fansIds) { fansIds_ = fansIds; }
    void setquestionPublishedIds(const string& questionPublishedIds) { questionPublishedIds_ = questionPublishedIds; }
    void setanswerPublishedIds(const string& answerPublishedIds) { answerPublishedIds_ = answerPublishedIds; }
    void setcommentPublishedIds(const string& commentPublishedIds) { commentPublishedIds_ = commentPublishedIds; }
    void setanswerUpvotedIds(const string& answerUpvotedIds) { answerUpvotedIds_ = answerUpvotedIds; }

    unordered_map<string, string> toMap()
    {
        unordered_map<string, string> userMap;
        userMap.insert(std::make_pair("userId", StringUtil::toString(userId_)));
        userMap.insert(std::make_pair("username", username_));
        userMap.insert(std::make_pair("password", password_));
        userMap.insert(std::make_pair("nickname", nickname_));
        userMap.insert(std::make_pair("articleIds", articleIds_));
        userMap.insert(std::make_pair("questionCollectedIds", questionCollectedIds_));
        userMap.insert(std::make_pair("questionFollowedIds", questionFollowedIds_));
        userMap.insert(std::make_pair("userFollowedIds", userFollowedIds_));
        userMap.insert(std::make_pair("fansIds", fansIds_));
        userMap.insert(std::make_pair("questionPublishedIds", questionPublishedIds_));
        userMap.insert(std::make_pair("answerPublishedIds", answerPublishedIds_));
        userMap.insert(std::make_pair("commentPublishedIds", commentPublishedIds_));
        userMap.insert(std::make_pair("answerUpvotedIds", answerUpvotedIds_));
        return userMap;
    }
private:
    int userId_;
    string username_;
    string password_;
    string nickname_;
    string articleIds_;
    string questionCollectedIds_;
    string questionFollowedIds_;
    string userFollowedIds_;
    string fansIds_;
    string questionPublishedIds_;
    string answerPublishedIds_;
    string commentPublishedIds_;
    string answerUpvotedIds_;
};
