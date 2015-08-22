/**
 * @file symbol.hpp
 * @author Miroslav Cibulka
 * @brief Symbol class
 */

#ifndef __BFP_SYMBOL_HPP
# define __BFP_SYMBOL_HPP


#ifndef BINARY_FILE_PARSER_BFP_HPP
# error "Don't include this file directly, use #include <bfp.hpp> instead"
#endif


namespace bfp
  {
      class File;

      class Symbol
        {
          /** Only File (factory method) may instantiate Symbol class */
          friend class File;

          /** Section may delete file */
          friend class Section;


      public:
          typedef Section *__section;
          typedef Instruction *__data;
          typedef ::std::vector<__data> __instr_vec;
          typedef Iterator<Symbol> __iterator;

          ~Symbol();

          void next(Instruction *_data);
          void prev(Instruction *_data);

          __iterator begin();

          __iterator end();

          size_t capacity();

          size_t size();

          size_t max_size();

          Instruction &operator[](
              size_t n);

          /////////////////////////////////////////////
          ///         Comparition operators         ///
          /////////////////////////////////////////////

          bool operator==(
              const Symbol &_compare);

          bool operator!=(
              const Symbol &_compare);

          bool operator==(
              const Symbol *_compare);

          bool operator!=(
              const Symbol *_compare);

          bool operator==(
              const symvalue _compare);

          bool operator!=(
              const symvalue _compare);

          bool operator==(
              const asymbol *_ptr);

          bool operator!=(
              const asymbol *_ptr);

          bool operator==(
              const char *_compare);

          bool operator!=(
              const char *_compare);

          friend bool operator==(
              const Symbol &_this,
              const Symbol &_compare);

          friend bool operator!=(
              const Symbol &_this,
              const Symbol &_compare);

          friend bool operator==(
              const Symbol &_this,
              const Symbol *_compare);

          friend bool operator!=(
              const Symbol &_this,
              const Symbol *_compare);

          friend bool operator==(
              const Symbol &_this,
              const symvalue _compare);

          friend bool operator!=(
              const Symbol &_this,
              const symvalue _compare);

          friend bool operator==(
              const Symbol &_this,
              const asymbol *_ptr);

          friend bool operator!=(
              const Symbol &_this,
              const asymbol *_ptr);

          friend bool operator==(
              const Symbol &_this,
              const char *_compare);

          friend bool operator!=(
              const Symbol &_this,
              const char *_compare);

          ///////////////////////////////
          ///         GETTERS         ///
          ///////////////////////////////

          /** @return string representation of symbol */
          const ::std::string getName() const;

          /** @return all sections where this symbol is (RO) - may be empty*/
          __section section();

          /** @return RAW value of symbol */
          symvalue getValue() const;

          /** @return vector of intructions */
          __instr_vec &&getInstructions();

          /////////////////////////////////////////////////
          ///             Symbol attributes             ///
          /////////////////////////////////////////////////

          bool hasFlags() const;

          bool isLocal() const;

          bool isGlobal() const;

          bool isExported() const;

          bool isFunction() const;

          bool isDebugging() const;

          bool isWeak() const;

          bool pointsToSection() const;

          bool isOldCommon() const;

          bool isNotAtEnd() const;

          bool isInConstructSection() const;

          bool isWarning() const;

          bool isIndirect() const;

          bool hasFileName() const;

          bool isFromDLI() const;

          bool hasObjectData() const;

      private:
          /**
           * @brief initialize symbol object
           * @param symbol bfd
           * @param parent File that this symbol belongs to
           */
          Symbol(
              asymbol *symbol,
              File *parent);

          /** Forbidden primitive constructor */
          Symbol() = delete;

          /** Forbidden copy constructor */
          Symbol(const Symbol &) = delete;

          /** Forbidden move constructor */
          Symbol(Symbol &&) = delete;

          /** Forbidden copy assignment */
          Symbol &operator=(const Symbol &) = delete;

          /** Forbidden move assignment */
          Symbol &operator=(Symbol &&) = delete;

      private:
          /** Appropriate Section to this symbol */
          __section _section = nullptr;

          /** BFD symbol structure */
          asymbol *_sym;

          /** File to which this symbol belongs to */
          File *_parent;

          /** Buffer for internal use */
          char *_buffer;

          /** Instruction vector */
          __instr_vec _instructions;

          /** if this symbol has any instructions to disassemble */
          bool has_no_intructions = false;

          int64_t _size = -1;
        };
  }

#endif //__BFP_SYMBOL_HPP
