import { Outlet } from "react-router";
export default function Layout(){
  function render(){
    return (
      <div id="layout">
          <nav>
              <a href="/">Sudoku solver</a> 
          </nav>
          <main>
            <Outlet/>
          </main>
          <footer>Marcin Perzanowski 2026</footer>
      </div>
    )
  }
  return render();
}