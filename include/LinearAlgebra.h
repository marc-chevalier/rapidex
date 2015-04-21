#pragma once

#include<vector>
#include<unordered_map>
#include<gmpxx.h>
#include<gmp.h>

class LinearAlgebra
{
public:
    template<typename T> static std::unordered_map<T, mpq_class> scalarMultiplication(mpq_class lhs, const std::unordered_map<T, mpq_class>& rhs) __attribute__((hot));
    template<typename T> static std::unordered_map<T, mpq_class> sum(const std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs) __attribute__((hot));
    template<typename T> static std::unordered_map<T, mpq_class> minus(const std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs) __attribute__((hot));
    template<typename T> static std::unordered_map<T, mpq_class> substitution(const std::unordered_map<T, mpq_class>& expression, const std::unordered_map<T, mpq_class>& substitutionExpression, T substitutedVariable) __attribute__((hot));
    template<typename T> static void simplify(std::unordered_map<T, mpq_class>& expression) __attribute__((hot));
};

template<typename T> std::unordered_map<T, mpq_class> LinearAlgebra::scalarMultiplication(mpq_class lhs, const std::unordered_map<T, mpq_class>& rhs)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;
    unordered_map_t output;

    if(lhs == 0)
        return output;

    for(const pair_t& terme : rhs)
        output[terme.first] = lhs * terme.second;

    return output;
}

template<typename T> std::unordered_map<T, mpq_class> LinearAlgebra::sum(const std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;
    unordered_map_t output;

    for(const pair_t& terme : lhs)
        output[terme.first] = terme.second;

    for(const pair_t& terme : rhs)
        if(output[terme.first] == -terme.second)
            output.erase(terme.first);
        else
            output[terme.first] += terme.second;

    simplify(output);

    return output;
}

template<typename T> std::unordered_map<T, mpq_class> LinearAlgebra::minus(const std::unordered_map<T, mpq_class>& lhs, const std::unordered_map<T, mpq_class>& rhs)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;
    unordered_map_t output;

    for(const pair_t& terme : lhs)
        output[terme.first] = terme.second;

    for(const pair_t& terme : rhs)
        if(output[terme.first] == terme.second)
            output.erase(terme.first);
        else
            output[terme.first] -= terme.second;

    return output;
}

template<typename T> std::unordered_map<T, mpq_class> LinearAlgebra::substitution(const std::unordered_map<T, mpq_class>& expression, const std::unordered_map<T, mpq_class>& substitutionExpression, T substitutedVariable)
{
    typedef typename std::unordered_map<T, mpq_class> unordered_map_t;
    typedef typename std::pair<T, mpq_class> pair_t;

    if(expression.count(substitutedVariable)==0)
        return expression;

    unordered_map_t output = expression;
    output.erase(substitutedVariable);

    if(expression.at(substitutedVariable) == 0)
        return output;

    for(const pair_t& terme : substitutionExpression)
        if(output[terme.first] == -expression.at(substitutedVariable)*terme.second)
            output.erase(terme.first);
        else
            output[terme.first] += expression.at(substitutedVariable)*terme.second;

    return output;
}

template<typename T> void LinearAlgebra::simplify(std::unordered_map<T, mpq_class>& expression)
{
    typedef typename std::unordered_map<T, mpq_class>::iterator it_t;
    std::vector<it_t> toErase;
    for(it_t it = expression.begin(); it != expression.end(); ++it)
        if(it->second == 0)
            toErase.push_back(it);

    for(const it_t& it : toErase)
        expression.erase(it);
}
